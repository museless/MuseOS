#ifndef _MUSE_PROC_H_
#define _MUSE_PROC_H_

/* Warning: one ant one job at a time*/

#define	NR_ANT		6
/* DF means dev and file */
#define	NR_DF		3
#define	NR_TEST		1
#define	NR_FAMILY	96

/* ss to eip, cpu did push */
struct	reg_stack {
	u32	edi;
	u32	esi;
	u32	ebp;
	u32	esp;
	u32	ebx;
	u32	edx;
	u32	ecx;
	u32	eax;
	u32	ds;
	u32	es;
	u32	fs;
	u32	gs;
	u32	ret_addr;

	u32	eip;
	u32	cs;
	u32	eflags;
	u32	ret_esp;
	u32	ss;
};

typedef	struct	process {
	struct	reg_stack	proc_part;		/* total 72 byte */
	struct	g_desc		proc_ldt[NR_LDT];	/* 24 byte */
	u16			proc_selector;

	struct	process		*proc_send_next;
	struct	process		*proc_recv_next;
	struct	process		*proc_alloc_heap_next;

	struct	message		*proc_msg;

	u32	proc_id;	/* thing id */
	u32	proc_state;	/* what is thing doing now */
	u32	proc_flags;	/* the attr for ant in env */

	u32	proc_priority;
	u32	proc_ticks;
	u32	proc_old_ticks;

	u32	proc_errno;	/* every thing has a errno */

	u32	proc_sendto;
	u32	proc_recvfrom;

	u32	proc_ntty;
	
	u32	proc_cwdir_ino;	/* current work dirent inode num */

}PROC;

typedef	struct	jobs {
	char	ant_name[DEF_NAMELEN];

	u32	stack;
	task_f	work;
}JOB;


/* the next data is the state to */
/* describe the ant's state */
#define	ALREADY_RUN		0x0
/* Soldier ant job */
#define	GET_DATA		0x1
#define	TRAN_DATA		0x2
#define	GET_HDINFO		0x3
/* Small worker ant job */
#define	CLEAN_NEST		0x4

#define	ESTABLISH_PORT		0x5
#define	CALLED			0x6

/* the flag in the PROC's flags */
#define	SENDING			0x2
#define	RECEVING		0x4
#define	LOCKED			0x8
#define	UNLOCK			0x10
#define	BLOCK			0x12
#define	UNBLOCK_MASK		0xFFFFFFED

/* used in syscall block */
/* block type */
#define	BLOCK_ONE		0x1
#define	UNBLOCK_TWO		0x2
#define	BLK_ONE_UN_TWO		0x3

#define	NONPROC			-1

/*----------global data----------*/
/* PER_FAMILY_SIZE for new family in the nursery */
#define	PER_FAMILY_SIZE		0x40000

#define	QUEEN_PLC		0x4000
#define	SOLDIER_PLC		0x4000
#define	SCOUT_PLC		0x4000
#define	MID_WORKER_PLC		0x4000
#define	SMALL_WORKER_PLC	0x4000
#define	NURSERY_ANT_PLC		0x4000

#define	DF_HD_PLC		0x4000
#define	DF_TTY_PLC		0x4000
#define	DF_MFS_PLC		0x8000

#define	TEST_PLC		0x4000

#define	ALL_PLC			(QUEEN_PLC + SOLDIER_PLC + SCOUT_PLC + MID_WORKER_PLC + SMALL_WORKER_PLC + \
				 NURSERY_ANT_PLC + \
				 DF_MFS_PLC + DF_HD_PLC + DF_TTY_PLC + \
				 TEST_PLC)


/* the next will used to init the ant */
PROC	pList[NR_ANT + NR_DF + NR_TEST];

PROC	*proc_on_view;
PROC	*proc_on_head;

int	reenter;
u8	jobStack[ALL_PLC];

/* function proto */
/* MuseKernel.asm */
void	proc_restart(void);

/* proc_app.c */
inline	void	proc_scheduling(void);
inline	int	proc_getpid(void);
inline	void	proc_cheating(u32 procOne, u32 procTwo);
inline	void	proc_caught_cheat(u32 procOne, u32 procTwo);
inline	void	proc_set_errno(u32 procOff, int nError);

/* syscall_block.c */
void	unblock(int procOff);

/* syscall_block.c */
void	sys_block(int procOne, int procTwo, int blockType, PROC *srcProc);

/* syscall_sendrecv.c */
int	sys_sendrec(int funType, int destOff, struct message *sendMsg, PROC *srcProc);

#endif
