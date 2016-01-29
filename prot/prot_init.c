#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "proto.h"

void	divide_error(void);
void	debug_abort(void);
void	nmi(void);
void	break_point(void);
void	overflow(void);
void	bounds_check(void);
void	inval_opcode(void);
void	no_math_fault(void);
void	double_fault(void);
void	inval_tss(void);
void	seg_not_present(void);
void	stack_fault(void);
void	general_prot(void);
void	page_fault(void);
void	math_fpu_fault(void);
void	align_check(void);
void	machine_check(void);
void	simd_fault(void);

void	intr0(void);
void	intr1(void);
void	intr2(void);
void	intr3(void);
void	intr4(void);
void	intr5(void);
void	intr6(void);
void	intr7(void);
void	intr8(void);
void	intr9(void);
void	intr10(void);
void	intr11(void);
void	intr12(void);
void	intr13(void);
void	intr14(void);
void	intr15(void);

void	system_call(void);


/*-----prot_gdesc_init-----*/
void prot_gdesc_init(struct g_desc *p, u32 base, u32 limit, u32 attr)
{
	limit &= 0xFFFFF;

	p->limit_low = limit & 0xFFFF;
	p->base_low = base & 0xFFFF;
	p->base_mid = (base >> 16) & 0xFF;
	p->attr1 = attr & 0xFF;
	p->attr2_limit_high = (((attr >> 8) & 0xF0) | ((limit >> 16) & 0x0F));
	p->base_high = (base >> 24) & 0xF;
}


/*-----prot_idesc_init-----*/
void prot_idesc_init(u32 desc, u8 priv, int_handler function, u8 attr)
{
	u32	base = (u32)function;

	nIdt[desc].offset_low = base & 0xFFFF;
	nIdt[desc].selector = SELECTOR_C;
	nIdt[desc].reserved = 0;
	nIdt[desc].attr = attr | priv;
	nIdt[desc].offset_high = (base >> 16) & 0xFFFF;
}


/*-----prot_itable_init-----*/
void prot_itable_init(void)
{
	prot_idesc_init(DE,		DPL0,
                	divide_error,	DA_386IGate);

	prot_idesc_init(DB,		DPL0,
                	debug_abort,	DA_386IGate);

	prot_idesc_init(NMI,  		DPL0,
			nmi,		DA_386IGate);

	prot_idesc_init(BP,		DPL3,
			break_point,	DA_386IGate);

	prot_idesc_init(OF,		DPL3,
			overflow,	DA_386IGate);

	prot_idesc_init(BR,		DPL0,
			bounds_check,	DA_386IGate);

	prot_idesc_init(UD,		DPL0,
			inval_opcode,	DA_386IGate);

	prot_idesc_init(NM,		DPL0,
			no_math_fault,	DA_386IGate);

	prot_idesc_init(DF,		DPL0,
			double_fault,	DA_386IGate);

	prot_idesc_init(TS,		DPL0,
			inval_tss,	DA_386IGate);

	prot_idesc_init(NP,		DPL0,
			seg_not_present,DA_386IGate);

	prot_idesc_init(SS,		DPL0,
			stack_fault,	DA_386IGate);

	prot_idesc_init(GP,		DPL0,
			general_prot,	DA_386IGate);

	prot_idesc_init(PF,		DPL0,
			page_fault,	DA_386IGate);

	prot_idesc_init(MF,		DPL0,
			math_fpu_fault,	DA_386IGate);

	prot_idesc_init(AC,		DPL0,
			align_check,	DA_386IGate);

	prot_idesc_init(MC,		DPL0,
			machine_check,	DA_386IGate);	

	prot_idesc_init(XF,		DPL0,
			simd_fault,	DA_386IGate);

	/* i8259A */
	prot_idesc_init(INTR0,		DPL0,
			intr0,		DA_386IGate);

	prot_idesc_init(INTR0 + 1,	DPL0,
			intr1,		DA_386IGate);

	prot_idesc_init(INTR0 + 2,	DPL0,
			intr2,		DA_386IGate);

	prot_idesc_init(INTR0 + 3,	DPL0,
			intr3,		DA_386IGate);

	prot_idesc_init(INTR0 + 4,	DPL0,
			intr4,		DA_386IGate);

	prot_idesc_init(INTR0 + 5,	DPL0,
			intr5,		DA_386IGate);

	prot_idesc_init(INTR0 + 6,	DPL0,
			intr6,		DA_386IGate);

	prot_idesc_init(INTR0 + 7,	DPL0,
			intr7,		DA_386IGate);

	prot_idesc_init(INTR8,		DPL0,
			intr8,		DA_386IGate);

	prot_idesc_init(INTR8 + 1,	DPL0,
			intr9,		DA_386IGate);

	prot_idesc_init(INTR8 + 2,	DPL0,
			intr10,		DA_386IGate);

	prot_idesc_init(INTR8 + 3,	DPL0,
			intr11,		DA_386IGate);

	prot_idesc_init(INTR8 + 4,	DPL0,
			intr12,		DA_386IGate);

	prot_idesc_init(INTR8 + 5,	DPL0,
			intr13,		DA_386IGate);

	prot_idesc_init(INTR8 + 6,	DPL0,
			intr14,		DA_386IGate);

	prot_idesc_init(INTR8 + 7,	DPL0,
			intr15,		DA_386IGate);

	prot_idesc_init(SYSCALL,	DPL3,
			system_call,	DA_386IGate | DPL3);

	int	i;

	/* tss */
	tss.ss0 = SELECTOR_D;
	prot_gdesc_init(&nGdt[DESC_TSS], (u32)&tss, sizeof(struct tss) - 1, DA_386TSS);

	for(i = 0; i < NR_ANT + NR_DF + NR_TEST; i++)
		prot_gdesc_init(&nGdt[DESC_LDT_F + i], (u32)(pList[i].proc_ldt), 3 * sizeof(struct g_desc) - 1, DA_LDT);

	return;
}


/*-----prot_hand_exception-----*/
void prot_hand_exception(u32 eDesc, u32 errCode, u32 rEip, u32 rCs, u32 eFlags)
{
	char	errMsg[][64]= {	"#DE Divide error",
                   		"#DB Debug error",
                   		"#--  NMI interrupt",
		   		"#BP Break point",
                   		"#OF Overflow",
                   		"#BD BOUND range exceeded",
                   		"#UD Inval opcode - like ud2 or sth",
                   		"#NM No math Fpu - Kidding? Right?",
                   		"#DF Double Fault",
                   		"#--  Nothing",
	          		"#TS Invalid TSS",
		   		"#NP Segment not present",
		   		"#SS Stack seg err - set ss wrong?",
		   		"#GP General prot - hard to find",
		   		"#PF Page fault - See you again",
		   		"#--  Nothing",
		   		"#MF x87 FPU Floating-point error",
		   		"#AC Alignment check",
		   		"#MC Machine check",
		   		"#XF SIMD Floating-point exception"};

	int	vCir;
	
	for(disp_pos = 0, vCir = 0; vCir < 80 * 4; vCir++)
		disp_color_str(" ", L_BLUE << 4);

	disp_pos = 0;
	disp_color_str(errMsg[eDesc], DEF_MSG_COLOR);

	if(errCode != 0xFFFFFFFF) {
		disp_color_str("\nErr_Code: ", DEF_MSG_COLOR);
		disp_int(errCode);
	}

	disp_color_str("\nEIP: ", DEF_MSG_COLOR);
	disp_int(rEip);
	disp_color_str("  CS: ", DEF_MSG_COLOR);
	disp_int(rCs);
	disp_color_str("\nEFLAGS: ", DEF_MSG_COLOR);
	disp_int(eFlags);

	if(eDesc == 14) {
		u32	nrReg = get_cr2();

		disp_color_str("  CR2: ", DEF_MSG_COLOR);
		disp_int(nrReg);
		nrReg = get_cr3();
		disp_color_str("  CR3: ", DEF_MSG_COLOR);
		disp_int(nrReg);
	}

	return;
}


/*-----prot_vatoma-----*/
/* vitrual addr to real memory addr */
void *prot_vatoma(int pID, void *offAddr)
{
	GDT	*gDesc = &pList[pID].proc_ldt[1];
	int	segBase = gDesc->base_low + (gDesc->base_mid << 16) + (gDesc->base_high << 24);

	return	(void *)(segBase + (u32)offAddr);
}
