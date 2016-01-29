#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "proto.h"


/*-----prot_i8259A_init-----*/
void prot_i8259A_init(void)
{
	out_byte(ICW1_M, 0x11, DELAY);
	out_byte(ICW1_S, 0x11, DELAY);

	out_byte(ICW2_M, 0x20, DELAY);
	out_byte(ICW2_S, 0x28, DELAY);

	out_byte(ICW3_M, 0x4, DELAY);
	out_byte(ICW3_S, 0x2, DELAY);

	out_byte(ICW4_M, 0x1, DELAY);
	out_byte(ICW4_S, 0x1, DELAY);
	
	out_byte(OCW1_M, 0xFF, DELAY);
	out_byte(OCW1_S, 0xFF, DELAY);

	int	i;

	for(i = 0; i < NR_INTR; i++)
		intr_call[i] = prot_default_intr;

	prot_itable_init();
}


/*-----prot_default_intr-----*/
void prot_default_intr(void)
{
	disp_color_str("\nintr happened\n", RED);
}


/*-----prot_set_ihandler-----*/
void prot_set_ihandler(int_handler handler, u32 desc)
{
	disable_intr(desc);
	intr_call[desc] = handler;
}
