#include "type.h"
#include "ctype.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"
#include "musefs.h"
#include "command.h"
#include "proto.h"


/* local function */
static	u32	musefs_get_dirinode(MESSAGE *fsMsg);
static	u32	musefs_rootino_info(u32 rootIno, char *pPath, int nDevice);
static	void	musefs_inode_writeback(int mOff, int nDevice);

static	void	musefs_server(MESSAGE *fsMsg);
static	void	musefs_queue_init(void);

/* local data */
static	u32	perBlkSect;
static	QUEUE	fsQueue;
static	u32	initFlags;

/* global data */
char	fsBuf[PER_BLK_SIZE_B];
LBUF	backBuf;
u32	fsTaker;

/* in sbList, museSb's info must at the first one */
MEMINO	inodeMap[MAX_INO_INMEM];
MDIR	rootDir;
PSB	sbList[MAX_SB];


/*-----musefs_platform-----*/
void musefs_platform(void)
{
	struct	message		fsMsg;

	perBlkSect = musefs_init();

	while(1) {
		send_recv(RECEIVE, MID_WORKER, &fsMsg);
		fsTaker = fsMsg.source;
		musefs_server(&fsMsg);
	}
}


/*-----musefs_server-----*/
static void musefs_server(MESSAGE *fsMsg)
{
	char	*pPath;
	u32	fInode, ctlType = fsMsg->type;

	if((fInode = musefs_get_dirinode(fsMsg)) == FAILURE)
		goto	musefs_sev_out;	

	pPath = musefs_get_dst(fsMsg->PATHNAME);

	if(ctlType & FS_CREAT)
		musefs_creat_file(fInode, pPath);

	musefs_sev_out:
	unblock(fsMsg->source);
}


/*-----musefs_get_dirinode-----*/
static u32 musefs_get_dirinode(MESSAGE *fsMsg)
{
	char	*pBuf = fsBuf;
	u32	rootIno;

	memset(fsBuf, '0', PER_BLK_SIZE_B);
	strncpy(pBuf, fsMsg->PATHNAME, MAX_PNAME);
	
	if(!(pBuf = musefs_cross_space(pBuf, strlen(pBuf)))) {
		proc_set_errno(fsTaker, EPATH);
		return	FAILURE;
	}

	rootIno = (*pBuf == '/') ? ROOT_INUM : pList[fsTaker].proc_cwdir_ino;
	return musefs_rootino_info(rootIno, pBuf, fsMsg->DEVICE);
}


/*-----musefs_rootino_info-----*/
static u32 musefs_rootino_info(u32 rootIno, char *pPath, int nDevice)
{
	MINODE	*pInode;
	MDIR	*pDir;
	u32	pathLen = 0;

	/* at this version, just has one inode map */
	if(rootIno > INODE_NUM)
		printx(MAGIC_KERN, "Inode num %x over Limit %x", rootIno, INODE_NUM);

	pPath += (ispath(*pPath)) ? 1 : 0;


	/* at every directory, "." is the first one and ".." is the second one */
	for(; rootIno < INODE_NUM && *pPath; pPath += pathLen, rootIno = pDir->d_inum) {
		pInode = musefs_get_inode(rootIno, nDevice);
		
		if((pPath = musefs_strip_path(pPath, &pathLen)) == NULL)
			break;

		if((pDir = musefs_find_directory(pInode, pPath, pathLen)) == NULL) {
			proc_set_errno(fsTaker, EUNEXISTPATH);
			return	FAILURE;
		}
	}

	return	rootIno;
}


/*-----musefs_get_inode-----*/
MINODE *musefs_get_inode(u32 inoNum, int nDevice)
{
	int	mOff, minOff = 1, minCou = inodeMap[1].i_linkcount;

	/* root inode always stay in inodeMap[0] */
	for(mOff = 0; inodeMap[mOff].i_istru.i_num != -1 && mOff < MAX_INO_INMEM; mOff++) {
		if(mOff != 0) {
			if(inodeMap[mOff].i_linkcount < minCou) {
				minOff = mOff;
				minCou = inodeMap[mOff].i_linkcount;
			}
		}

		if(inodeMap[mOff].i_istru.i_num == inoNum) {
			inodeMap[mOff].i_linkcount++;
			return	(&inodeMap[mOff].i_istru);
		}
	}

	if(inodeMap[minOff].i_dirty == DIRTY)
		musefs_inode_writeback(minOff, nDevice);
	
	mOff = musefs_inode_lba(inodeMap[minOff].i_istru.i_num) + sbList[0].psb_stru.sb_ilist_off;
	hd_wrdata(nDevice, mOff, sizeof(MINODE), (char *)(&inodeMap[minOff].i_istru), READ);
	inodeMap[minOff].i_dirty = UNDIRTY;
	inodeMap[minOff].i_linkcount = 1;
	
	return	(&inodeMap[minOff].i_istru);
}


/*-----musefs_get_emptyinode-----*/
MINODE *musefs_get_emptyinode(void)
{
	int	imOff, lessUse = 1;
	
	for(imOff = 1; imOff < MAX_INO_INMEM; imOff++) {
		if(inodeMap[lessUse].i_linkcount == -1)
			return	&inodeMap[imOff].i_istru;
	
		if(inodeMap[lessUse].i_linkcount < inodeMap[imOff].i_linkcount)
			lessUse = imOff;
	}

	musefs_inode_writeback(imOff, MUSEFS_DEV);
	return	&inodeMap[imOff].i_istru;
}


/*-----musefs_inode_writeback-----*/
static void musefs_inode_writeback(int mOff, int nDevice)
{
	int	iLba;

	iLba = musefs_inode_lba(inodeMap[mOff].i_istru.i_num);
	iLba += sbList[0].psb_stru.sb_ilist_off;
	
	hd_wrdata(nDevice, iLba, sizeof(MINODE), (char *)(&inodeMap[mOff].i_istru), WRITE);
}


/*-----musefs_queue_init-----*/
static void musefs_queue_init(void)
{
	fsQueue.buf_head = fsQueue.buf_tail = fsQueue.buf;
	fsQueue.buf_count = NR_IMBUF;
}


/*-----musefs_access_ctl-----*/
void musefs_access_ctl(u32 reqCom, struct message *tranMsg)
{
	PROC	*pDest = (proc_on_head + MID_WORKER);

	if(!initFlags)
		musefs_queue_init();

	access_mutex_lock(&fsQueue);

	pDest->proc_state = reqCom;
	pDest->proc_msg = ((tranMsg) ? tranMsg : NULL);

	block(tranMsg->source, MID_WORKER, BLK_ONE_UN_TWO);

	access_mutex_unlock(&fsQueue);
}
