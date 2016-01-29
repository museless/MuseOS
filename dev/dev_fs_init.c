#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"
#include "musefs.h"
#include "command.h"
#include "proto.h"


/* local function */
static	void	musefs_init_map(void);


/*-----musefs_init-----*/
int musefs_init(void)
{
	MESSAGE	devMsg;
	MSB	museSb;
	PLBA	devInfo;
	u32	tranLba, tiSect, perBlkSect;
	
	musefs_init_map();
	
	devMsg.BUF = &devInfo;
	devMsg.DEVICE = MINOR(MUSEFS_DEV);
	devMsg.source = DF_MFS;
	devMsg.type = IOCTL;
	
	hd_access_ctl(GET_HDINFO, &devMsg);
	
	/* init super block */
	museSb.sb_magic = MAGIC_MFS;
	museSb.sb_initflags = INITFLAG_MFS;
	museSb.sb_size = PER_SB_SIZE;
	perBlkSect = museSb.sb_block_size = (devInfo.pl_size > BIG_HARD_DRIVE) ? PER_BLK_SIZE_B : PER_BLK_SIZE_S;
	perBlkSect /= PER_SECT_SIZE;

	museSb.sb_total_inode = INODE_NUM;
	museSb.sb_total_sector = devInfo.pl_size;
	museSb.sb_per_inode_size = PER_INODE_SIZE;
	museSb.sb_bmap_off = BMAP_OFF;
	museSb.sb_bmap_size = ((museSb.sb_total_sector >> PER_BITS_SHF) / museSb.sb_block_size + 1);

	museSb.sb_imap_off = BMAP_OFF + museSb.sb_bmap_size;
	museSb.sb_imap_size = perBlkSect;
	museSb.sb_ilist_off = museSb.sb_imap_off + museSb.sb_imap_size;
	tiSect = (museSb.sb_per_inode_size * ((museSb.sb_total_inode + 1) >> PER_SSIZE_SFT));
	museSb.sb_ilist_size = tiSect;
	museSb.sb_data_off = museSb.sb_ilist_off + tiSect;
	
	museSb.sb_free_inode = museSb.sb_total_inode - 1;
	museSb.sb_free_sector = museSb.sb_total_sector - museSb.sb_data_off;
	
	/* write super block */
	tranLba = SB_OFF;
	hd_wrdata(MUSEFS_DEV, tranLba, PER_SB_SIZE, (char *)&museSb, WRITE);

	/* init blockmap */
	u32	bOff, bitOff = 3, bSize;
	
	memset(fsBuf, 0, museSb.sb_block_size);
	/* total 1 + 1 + 1 + tiSect was used */
	fsBuf[0] = 0x7;

	for(bSize = bOff = 0; bSize < tiSect; bSize++, bitOff++) {
		if(bitOff == 8) {
			bitOff = 0;
			bOff++;
		}
	
		fsBuf[bOff] |= (1 << bitOff);
	}
	
	/* write blockmap */
	tranLba = BMAP_OFF;
	hd_wrdata(MUSEFS_DEV, tranLba, museSb.sb_block_size, (char *)fsBuf, WRITE);

	/* init inode map */
	memset(fsBuf, 0, museSb.sb_block_size);
	/* at first, just have one inode "Root" */
	fsBuf[0] |= 1;
	
	tranLba = museSb.sb_imap_off;
	hd_wrdata(MUSEFS_DEV, tranLba, museSb.sb_block_size, (char *)fsBuf, WRITE);
	
	/* init first inode "Root" */
	MEMINO	*pRoot = &inodeMap[0];
	
	pRoot->i_istru.i_type = SYSTEM_DIR;
	pRoot->i_istru.i_state = INO_TAKEN;
	pRoot->i_istru.i_nblocks = pRoot->i_istru.i_num = 0;
	pRoot->i_istru.i_nsize = sizeof(MDIR);
	pRoot->i_istru.i_dtype = DAT_INSIDE;
	pRoot->i_linkcount = 1;
	
	rootDir.d_inum = 0;
	strncpy(rootDir.d_name, ".", 1);
	
	memcpy(pRoot->i_istru.i_data, (char *)&rootDir, sizeof(MDIR));
	
	tranLba = museSb.sb_ilist_off;
	hd_wrdata(MUSEFS_DEV, tranLba, sizeof(MINODE), (char *)(&pRoot->i_istru), WRITE);
	
	sbList[0].psb_drive = MUSEFS_DEV;
	sbList[0].psb_stru = museSb;
	
	return	perBlkSect;
}


/*-----musefs_init_map-----*/
static void musefs_init_map(void)
{
	int	mOff;

	for(mOff = 0; mOff < MAX_INO_INMEM; mOff++)
		inodeMap[mOff].i_istru.i_num = -1;
}
