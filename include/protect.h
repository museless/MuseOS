#ifndef	_MUSE_PROTECT_H_
#define	_MUSE_PROTECT_H_

/* macro */
#define	DT_REG_SIZE	0x6

/* the next define used to init the i8259A */
#define	ICW1_M		0x20
#define	ICW1_S		0xA0
#define	ICW2_M		0x21
#define	ICW2_S		0xA1
#define	ICW3_M		ICW2_M
#define	ICW3_S		ICW2_S
#define	ICW4_M		ICW2_M
#define	ICW4_S		ICW2_S	

#define	OCW1_M		ICW2_M
#define	OCW1_S		ICW2_S

#define	OCW2_M		ICW2_M
#define	OCW2_S		ICW2_S

#define	EOI		0x20

/* gdt descriptor */
typedef	struct g_desc {
	u16	limit_low;
	u16	base_low;
	u8	base_mid;
	u8	attr1;
	u8	attr2_limit_high;
	u8	base_high;
}GDT;

/* GDT Num */
#define	GDT_INDEX	0x0
#define	DESC_CODE	0x1
#define	DESC_DATA	0x2
#define	DESC_VIDEO	0x3
#define	DESC_TSS	0x4
#define	DESC_LDT_F	0x5

/* SELECTOR */
#define	SELECTOR_C	0x8
#define	SELECTOR_D	0x10
#define	SELECTOR_V	0x1B
#define	SELECTOR_TSS	0x20
#define	SELECTOR_LDT_F	0x28

/* idt descriptor */
struct i_desc {
	u16	offset_low;
	u16	selector;
	u8	reserved;
	u8	attr;
	u16	offset_high;
};

/* task statement seg */
/* Muse os just used esp0 & ss0 */
struct tss {
	u32	backline;
	u32	esp0;
	u32	ss0;
	u32	esp1;
	u32	ss1;
	u32	esp2;
	u32	ss2;
	u32	cr3;
	u32	eip;
	u32	eflags;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	edi;
	u32	esi;
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	gs;
	u32	fs;
	u32	ldt;
	u16	trap;
	u16	io_base;
};

/* used in protect mode */
#define	DA_32		0x4000
#define DA_LIMIT_4K	0x8000	

#define DPL0		0x00
#define DPL1		0x20
#define DPL2		0x40
#define DPL3		0x60

#define DA_DR		0x90
#define DA_DRW		0x92	
#define DA_DRWA		0x93	
#define DA_C		0x98	
#define DA_CR		0x9A	
#define DA_CCO		0x9C	
#define DA_CCOR		0x9E	

#define DA_LDT		0x82
#define DA_TaskGate	0x85	
#define DA_386TSS	0x89
#define DA_386CGate	0x8C
#define DA_386IGate	0x8E
#define DA_386TGate	0x8F

#define SA_RPL0		0
#define SA_RPL1		1
#define SA_RPL2		2
#define SA_RPL3		3

#define SA_TIG		0
#define SA_TIL		4

/* idt num */
#define	DE		0
#define	DB		1
#define	NMI		2
#define	BP		3
#define	OF		4
#define	BR		5
#define	UD		6
#define	NM		7
#define	DF		8
#define	TS		10
#define	NP		11
#define	SS		12
#define	GP		13
#define	PF		14
#define	MF		16
#define	AC		17
#define	MC		18
#define	XF		19

/* intr num */
#define	INTR0		0x20
#define	INTR8		0x28
#define	SYSCALL		0x80

/* function proto */

/* i8259A.c */
void	prot_i8259A_init(void);
void	prot_default_intr(void);
void	prot_set_ihandler(int_handler handler, u32 desc);

/* protect.c */
void	prot_itable_init(void);
void	prot_gdesc_init(struct g_desc *p, u32 base, u32 limit, u32 attr);
void	prot_idesc_init(u32 desc, u8 priv, int_handler function, u8 attr);
void	prot_hand_exception(u32 eDesc, u32 errCode, u32 rEip, u32 rCs, u32 eFlags);
void	*prot_vatoma(int tid, void *addr);

/* global data */
PUBLIC	u8	pGdt[DT_REG_SIZE];
struct	g_desc	nGdt[NR_GDT];


PUBLIC	u8	pIdt[DT_REG_SIZE];
struct	i_desc	nIdt[NR_IDT];

int_handler	intr_call[NR_INTR];

struct	tss	tss;

#endif
