#ifndef	_MUSE_FS_H
#define	_MUSE_FS_H

#include "fs.h"

/* macro */
#define	MAGIC_MFS	0x20
#define	INITFLAG_MFS	MAGIC_MFS
#define	MAX_SB		0x8
#define	MAX_FSIZE	0x10000

#define	MAX_INO_INMEM	0x10
#define	MAX_DIR_INMEM	0x10

#define	UNDIRTY		0x0
#define	DIRTY		0x1

typedef	struct	musefs_super_block_h {
/*0x0*/	u16	sb_magic;
	u16	sb_initflags;
	u32	sb_size;
	u32	sb_block_size;
	u32	sb_total_inode;

/*0x10*/u32	sb_total_sector;
	u32	sb_per_inode_size;
	u32	sb_bmap_off;
	u32	sb_imap_off;

/*0x20*/u32	sb_ilist_off;
	u32	sb_data_off;
	u32	sb_bmap_size;
	u32	sb_imap_size;

/*0x30*/u32	sb_ilist_size;
	u32	sb_free_inode;
	u32	sb_free_sector;

/*0x3C*/u32	sb_reserved[0xF1];

}MSB;

typedef	struct	musefs_p_sb {
	u32	psb_drive;
	u32	psb_dirty;
	MSB	psb_stru;
}PSB;

#define	PER_SB_SIZE	sizeof(MSB)
#define	PER_INODE_SIZE	0x200

/* a temp num */
#define	BIG_HARD_DRIVE	0x7FFFFFFF

#define	PER_BLK_SIZE_S	0x400
#define	PER_BLK_SIZE_B	0x1000

#define	INODE_NUM	0x800
#define	INODE_DSIZE	0x100

#define	ROOT_INUM	0x0

typedef	struct	musefs_inode {
	u16	i_type;
	u16	i_state;
	u32	i_num;
	u32	i_nsize;
	u32	i_nblocks;
/*0x10*/u16	i_dtype;	/* data type DAT_INSIDE or DAT_OUTSIDE */
	u8	i_reserved[0xEE];
	u8	i_data[INODE_DSIZE];
}MINODE;

typedef	struct {
	int	i_linkcount;
	u32	i_dirty;
	MINODE	i_istru;
}MEMINO;

/* i_type */
#define	NORMAL_FILE	0x4E46
#define	NORMAL_DIR	0x4E44
#define	SYSTEM_FILE	0x6346
#define	SYSTEM_DIR	0x6344

/* i_state */
#define	INO_UNTAKE	0x0
#define	INO_TAKEN	0x1
#define	INO_ABAND	0x2

/* i_dtype */
#define	DAT_INSIDE	0x494E
#define	DAT_OUTSIDE	0x4F55

/* use lba num */
#define	SB_OFF		0x0
#define	BMAP_OFF	0x2

typedef	struct	musefs_dirent {
	u32	d_inum;
	char	d_name[MAX_DNAME];
}MDIR;

/* msg type in fs */
#define	FS_RDWR		0x10
#define	FS_TRUNC	0x20
#define	FS_CREAT	0x40
#define	FS_OPEN		0x80
#define	FS_REMOVE	0x100

#define	FS_STMASK	0xFF0

/* lba buf */
typedef struct {
	u32	l_addr;
	char	l_buf[PER_BLK_SIZE_B];
}LBUF;

/* extern data */
extern	PSB	sbList[MAX_SB];
extern	char	fsBuf[PER_BLK_SIZE_B];
extern	LBUF	backBuf;
extern	MEMINO	inodeMap[MAX_INO_INMEM];
extern	MDIR	rootDir;
extern	u32	fsTaker;

/* macro function */
/* because inode's size is always 512byte, so it equal one lba's size */
#define	musefs_inode_lba(iNum)	(iNum)

/* global function */
/* musefs_main.c */
void	musefs_access_ctl(u32 reqCom, struct message *tranMsg);
MINODE	*musefs_get_inode(u32 inoNum, int nDevice);
MINODE	*musefs_get_emptyinode(void);

/* musefs_misc.c */
char	*musefs_cross_space(char *pBuf, int bLimit);
char	*musefs_strip_path(char *fPath, u32 *pLen);
char	*musefs_get_dst(char *pName);
u32	musefs_block_to_lba(u32 nBlock);

/* musefs_file_ctl.c */
void	musefs_creat_file(u32 rootIno, char *fName);
MDIR	*musefs_find_directory(MINODE *dInode, char *pStr, int pLen);

#endif
