#ifndef	_MUSE_NEST_H_
#define	_MUSE_NEST_H_

typedef	struct	nest_stru {
	PROC	*ns_wait;
	u32	ns_size;		/* count for byte */
	u32	ns_state;

	u32	ns_charge_id;		/* which ant was the manager */

	union	statistic {		/* count for page number */
		u32	nheap;
		u32	nfamily;
		u32	nbuffer;
	}ns_data;

	char	ns_stru_name[16];
	char	ns_magic;
}NEST;

#define	Magic_F_S_1	'\001'
#define	Magic_N_1	'\002'
#define	Magic_P_N_1	'\003'

/* used in page mode */
#define	PAGE_DIR_BASE	0x2000
#define	PAGE_ENT_BASE	0x100000

#define	PG_SU_LV_MASK	0xFFFFFFFB

#define	ONLY_READ_MASK	0xFFFFFFFD
#define	RW_MASK		0x00000002

/* Total memory size 64M, you can change */
#define	MEMORY_SIZE	0x4000000
/* 64M - 5M = 59M for nest size */
#define	NEST_SIZE	0x3B00000
/* food store room size 10M */
#define	BUFFER_STORE_SIZE	0xA00000
/* nursery room size 24M */
#define	NURSERY_SIZE	0x1800000
/* other room size 25M */
#define	HEAP_SIZE	0x1900000
/* page table size 4M */
#define	PAGE_TABLE_SIZE	0x400000
/* Kernel size 1M */
#define	KERNEL_SIZE	0x100000
/* Except OTHER_SIZE & NURSERY_SIZE & BUFFER_STORE_SIZE */
#define	BUFFER_OFF	(KERNEL_SIZE + PAGE_TABLE_SIZE)
/* Except OTHER_SIZE & NURSERY_SIZE */
#define	NURSERY_OFF	(KERNEL_SIZE + PAGE_TABLE_SIZE + BUFFER_STORE_SIZE)
/* Except OTHER_SIZE */
#define	HEAP_OFF	(KERNEL_SIZE + PAGE_TABLE_SIZE + NURSERY_SIZE + BUFFER_STORE_SIZE)

#define	PER_PAGE_BYTE	(0x400 << 2)
#define	NR_PAGE		(MEMORY_SIZE / PER_PAGE_BYTE)

/* page locate */
#define	NUR_PAGE_LOC	0x109C00
#define	HEAP_PAGE_LOC	0x109C04
#define	BUFFER_PAGE_LOC	0x101400

/* nest_stru state */
#define	FULL		0x2
#define	COUNTING	0x1
#define	ALLOW		0x0

/* page state */
#define	USED		0x1
#define	FREE		0x0

/* the next thing used for food */
#define	PER_BUFFER_BYTE	0x400
/* food store (buffer) start addr 0x50000(5M) and add one page (4KB) */
#define	BUFFER_RL_ADDR	(BUFFER_OFF + PER_PAGE_BYTE)

typedef	struct	food_blk {
	struct	food_blk	*p_hash_head;
	struct	food_blk	*p_hash_next;
	struct	food_blk	*p_list_head;
	struct	food_blk	*p_list_next;
	struct	body		*p_wait;

	char	*data;
	
	unsigned int		dev_nr;
	unsigned int		sector_nr;

	unsigned char		b_lock;
	unsigned char		b_dirt;
	unsigned char		b_update;
	unsigned char		user_count;
}FDBLK;

#define	BUFFER_STRU_LEN	sizeof(struct food_blk)
#define	NR_BUFFER_HASH	137
#define	HASHLIST_LEN	(NR_BUFFER_HASH * BUFFER_STRU_LEN)
#define	NR_BUFFER_BLK	((BUFFER_STORE_SIZE - ONE_PAGE_SIZE - HASHLIST_LEN) / (BUFFER_STRU_LEN + PER_BUFFER_BYTE))
#define	NR_BUFFER_SIZE	(NR_BUFFER_BLK * PER_BUFFER_BYTE)

#define	BUFFER_HL_OFF	(BUFFER_RL_ADDR)
#define	BUFFER_LIST_OFF	(BUFFER_HL_OFF + HASHLIST_LEN)
#define	BUFFER_RL_OFF	(BUFFER_LIST_OFF + (NR_BUFFER_BLK * BUFFER_STRU_LEN))

/* data */
NEST	*pBuffer;
NEST	*pNursery;
NEST	*pHeap;

u16	pageBuf[NR_PAGE];
/* in famStBuf has two part */
/* high 7 bits was proc id */
/* low 1 bit was page state */
u8	famStBuf[NR_FAMILY];

/* function proto */
/* file_memory_ctl.c */
void	*mem_alloc_heap(int size);
int	mem_allow_enter_heap(void);
int	mem_free_heap(void *addr);

void	*mem_alloc_nursery(int pID);
int	mem_allow_enter_nursery(void);
int	mem_free_nursery(int pID);

int 	mem_nursery_find_proc(int tid, int *locate);

/* file_memory_protect.c */
int	mem_set_nest_protection(void);
void	mem_buffer_close(void);
void	mem_nursery_close(void);
void	mem_heap_close(void);
int	mem_heap_allow_write(void);
int	mem_nursery_allow_write(void);

#endif
