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
static	int	musefs_make_ndir(u32 rootIno, char *fName);
static	u32	musefs_alloc_ninode(void);
static	void	musefs_make_norfile(u32 inoNum);

/* local inline */
static	inline	void	musefs_read_imap(void);


/*-----musefs_creat_file-----*/
void musefs_creat_file(u32 rootIno, char *fName)
{
	u32	fInode;

	if((fInode = musefs_make_ndir(rootIno, fName)) == FAILURE)
		return;

	musefs_make_norfile(fInode);
}


/*-----musefs_make_ndir-----*/
static int musefs_make_ndir(u32 rootIno, char *fName)
{
	MINODE	*pInode = musefs_get_inode(rootIno, MUSEFS_DEV);
	MDIR	*pDir;
	u32	fInode;

	if(MAX_FSIZE - pInode->i_nsize < sizeof(MDIR)) {
		proc_set_errno(fsTaker, EFSIZEOUT);
		return	FAILURE;
	}

	if((fInode = musefs_alloc_ninode()) == FAILURE)
		return	FAILURE;

	if((pDir = musefs_find_directory(pInode, NULL_STR, 1)) == NULL) {
		proc_set_errno(fsTaker, EMEMOUT);
		return	FAILURE;
	}

	pDir->d_inum = fInode;
	strncpy(pDir->d_name, fName, strlen(fName));

	return	fInode;
	
// 	hd_wrdata(MUSEFS_DEV, sbList[0].psb_stru.sb_imap_off, sbList[0].psb_stru.sb_block_size, fsBuf, WRITE);
// 	hd_wrdata(MUSEFS_DEV, sbList[0].psb_stru.sb_ilist_off + pInode->i_num, sizeof(MINODE), (char *)pInode, WRITE);
// 	hd_wrdata(MUSEFS_DEV, sbList[0].psb_stru.sb_ilist_off + fInode, sizeof(MINODE), (char *)nInode, WRITE);
// 
// 	printx(MAGIC_KERN, "%x, %x, %d", pInode->i_data, pDir, fInode);
}


/*-----musefs_make_norfile-----*/
static void musefs_make_norfile(u32 inoNum)
{
	nInode = musefs_get_emptyinode();
	nInode->i_type = NORMAL_FILE;
	nInode->i_state = INO_TAKEN;
	nInode->i_num = fInode;
	nInode->i_nblocks = pInode->i_nsize = 0;
	nInode->i_dtype = DAT_INSIDE;
}


/*-----musefs_alloc_ninode-----*/
static u32 musefs_alloc_ninode(void)
{
	u32	sByte, bitOff;

	if(!sbList[0].psb_stru.sb_free_inode) {
		proc_set_errno(fsTaker, EINODEOUT);
		return	FAILURE;
	}

	musefs_read_imap();

	for(sByte = 0; sByte < sbList[0].psb_stru.sb_total_inode >> PER_BITS_SHF; sByte++) {
		if(fsBuf[sByte] != 0xF) {
			for(bitOff = 0; bitOff < PER_BITS_BYTE; bitOff++) {
				if(!(fsBuf[sByte] & (1 << bitOff))) {
					fsBuf[sByte] |= (1 << bitOff);
					printx(MAGIC_USR, "sByte: %d\n", fsBuf[sByte]);
					return	((sByte << PER_BITS_SHF) + bitOff);
				}
			}
		}
	}

	proc_set_errno(fsTaker, EFSERR);
	return	FAILURE;
}


/*-----musefs_find_directory-----*/
MDIR *musefs_find_directory(MINODE *dInode, char *pStr, int pLen)
{
	u8	*pData = dInode->i_data;
	u32	nSize, readByte = sbList[0].psb_stru.sb_block_size;

	if(dInode->i_type != NORMAL_DIR && dInode->i_type != SYSTEM_DIR) {
		proc_set_errno(fsTaker, EPATH);
		return	NULL;
	}

	if(dInode->i_dtype == DAT_INSIDE) {
		for(; pData < dInode->i_data + INODE_DSIZE; pData += sizeof(MDIR)) {
			if(!strncmp(((MDIR *)pData)->d_name, pStr, pLen))
				return	(MDIR *)pData;
		}

	} else if(dInode->i_dtype == DAT_OUTSIDE) {
		char	*dDir = backBuf.l_buf;

		for(; pData < dInode->i_data + INODE_DSIZE && *(u32 *)pData != 0; pData += sizeof(u32)) {
			hd_wrdata(MUSEFS_DEV, musefs_block_to_lba(*(u32 *)pData), readByte, dDir, READ);
			backBuf.l_addr = musefs_block_to_lba(*(u32 *)pData);
			
			for(nSize = 0; nSize < readByte; nSize += sizeof(MDIR)) {
				if(!strncmp(((MDIR *)(dDir + nSize))->d_name, pStr, pLen))
					return	(MDIR *)dDir;
			}
		}

	} else {
		printx(MAGIC_KERN, "Unknown i_dtype: %d\n", dInode->i_dtype);
	}

	return	NULL;
}


/*-----musefs_read_imap-----*/
static inline void musefs_read_imap(void)
{
	hd_wrdata(MUSEFS_DEV, sbList[0].psb_stru.sb_imap_off, 
		  sbList[0].psb_stru.sb_imap_size << PER_SSIZE_SFT, fsBuf, READ);
}

