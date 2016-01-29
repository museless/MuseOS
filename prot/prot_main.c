#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "proto.h"

/*-----prot_main-----*/
void prot_main(void)
{
	u32*	gBase;
	u16*	gLimit;

	disp_color_str("\n\n\n\n\n\n\n\n\n\n", RED);
	
	/* init gdt */
	gBase = (u32 *)(pGdt + 2);
	gLimit = (u16 *)(pGdt + 0);
	memcpy(nGdt, (void *)(*gBase), *gLimit + 1);

	*gBase = (u32)&nGdt;
	*gLimit = NR_GDT * sizeof(struct g_desc) - 1;

	/* init idt */
	prot_i8259A_init();

	gBase = (u32 *)(pIdt + 2);
	gLimit = (u16 *)(pIdt + 0);

	*gBase = (u32)&nIdt;
	*gLimit = NR_IDT * sizeof(struct i_desc) - 1;
}
