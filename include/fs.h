#ifndef	_FS_H
#define	_FS_H

/* macro */
#define	MAX_PNAME	0x100
#define	MAX_DNAME	0x3C

#define	PER_BITS_BYTE	0x8
#define	PER_BITS_SHF	0x3

#define	PER_SECT_SIZE	0x200
#define	PER_SSIZE_SFT	9

#define	NULL_STR	"\0"

typedef	struct	fstat {
	u32	st_inum;
	u32	st_size;
	u16	st_type;
}FSTAT;

#endif
