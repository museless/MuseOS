#ifndef	_MUSE_HD_H_
#define	_MUSE_HD_H_

struct	hd_com_reg {
	u8	hc_features;
	u8	hc_sector_count;
	u8	hc_lba_low;
	u8	hc_lba_mid;
	u8	hc_lba_high;
	u8	hc_device;
	u8	hc_command;
};

typedef	struct	part_lba {
	u32	pl_base;
	u32	pl_size;
}PLBA;

struct	partition {
	u8	pt_status;
	u8	pt_start_head;
	u8	pt_start_sector;
	u8	pt_start_cyl;
	u8	pt_system_id;
	u8	pt_end_head;
	u8	pt_end_sector;
	u8	pt_end_cyl;

	PLBA	pt_lba_info;
};

/* major dev list */
#define	DEV_HD		0x1

/* misc */
#define	HD_TIMEOUT	10000
#define	PART_TAB_OFF	0x1BE

#define	HD_OPEN		0x1
#define	HD_CLOSE	0x2

/* read */
#define	P_DATA		0x1F0
#define	P_ERROR		0x1F1
#define	P_SECTOR_COUNT	0x1F2
#define	P_LBA_LOW	0x1F3
#define	P_LBA_MID	0x1F4
#define	P_LBA_HIGH	0x1F5
#define	P_DEVICE	0x1F6
#define	P_STATUS	0x1F7
#define	P_ALTERNATE_S	0x3F6

/* write */
#define	P_FEATURES	0x1F1
#define	P_COMMAND	0x1F7
#define	P_DEV_CONTROL	0x3F6

/* command */
#define	IDENTIFY_COM	0xEC

#define	WRITE_COM	0x30
#define	READ_COM	0x20

/* status reg */
#define	BSY_SET		0x80
#define	TRAN_SET	0x08

/* drive device num */
#define	MAX_HD		2
#define	NR_PART_PER_HD	4
#define	NR_LOG_PER_PART	16
#define	NR_LOG_PER_HD	(NR_LOG_PER_PART * NR_PART_PER_HD)
#define	NR_PRIM_PER_HD	(NR_PART_PER_HD + 1)

#define	MAX_PRIM	(NR_PRIM_PER_HD * MAX_HD - 1)
#define	MAX_LOG		(NR_LOG_PER_HD * MAX_HD)

#define	MAJOR(x)	(x >> 16)
#define	MINOR(x)	(x & 0xFF)

#define	NDRIVE(x)	((x < MAX_PRIM) ? (x / NR_PART_PER_HD) : ((x - ROOT_1a) / NR_PRIM_PER_HD))
#define	MDEV(d, p)	((d << 16) | p)

#define	ROOT_1a		0x10
#define	ROOT_2a		(ROOT_1a + NR_LOG_PER_PART)
#define	ROOT_3a		(ROOT_2a + NR_LOG_PER_PART)
#define	ROOT_4a		(ROOT_3a + NR_LOG_PER_PART)
#define	MUSEFS_DEV	MDEV(DEV_HD, (ROOT_2a + 2))

#define	BYTE_PER_SECT	0x200

/* data len */
/* identify device data len */
#define	IDEN_DEV_DAT_L	0x200
#define	PART_TABLE_LEN	0x40
#define	ONE_PART_TAB_L	0x10

/* system id in part table */
#define	EXTENDED	0x5

#define	MAKE_DEV_REG(mode,drive,lba) ((mode << 6) | (drive << 4) | ((lba >> 24) & 0xF) | 0xA0)

/* function proto */
/* dev_hd_ctl.c */
void	hd_handler(void);
void	hd_platform(void);

void	dev_ready(int procOff);
void	hd_wrdata(int nDrive, int nLba, int bSize, char *rBuf, int cType);

/* dev_fs_init.c */
int	musefs_init(void);

#endif
