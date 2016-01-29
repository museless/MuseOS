#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "proto.h"

/*-----Muse_main-----*/
void Muse_main(void)
{
	PROC	*pPTab = pList;
	JOB	*pJTab = jList;
	u8	*pStack = jobStack + ALL_PLC;
	int	lCir;

	for(lCir = 0; lCir < NR_ANT + NR_DF + NR_TEST; lCir++, pPTab++, pJTab++) {
		pPTab->proc_part.es = pPTab->proc_part.fs = pPTab->proc_part.ds = pPTab->proc_part.ss = 0x15;
		pPTab->proc_part.gs = SELECTOR_V;
		pPTab->proc_part.cs = 0xD;
		pPTab->proc_part.eip = (u32)pJTab->work;
		pPTab->proc_part.ret_esp = (u32)pStack;
		pPTab->proc_part.eflags = 0x1202;

		pPTab->proc_selector = SELECTOR_LDT_F + 8 * lCir;

		pPTab->proc_ticks = pPTab->proc_priority = DEFAULT_TICKS;
		pPTab->proc_state = 0;
		pPTab->proc_id = lCir;
		pPTab->proc_errno = 0;
		pPTab->proc_flags = 0;

		pPTab->proc_ntty = 0;

		pPTab->proc_recvfrom = pPTab->proc_sendto = NO_TASK;
		pPTab->proc_send_next = pPTab->proc_recv_next = NULL;
		pPTab->proc_msg = NULL;

		prot_gdesc_init(&pPTab->proc_ldt[0], 0, 0, 0);
		prot_gdesc_init(&pPTab->proc_ldt[1], 0, 0xFFFFF, DA_LIMIT_4K | DA_32 | DPL1 | DA_CR);
		prot_gdesc_init(&pPTab->proc_ldt[2], 0, 0xFFFFF, DA_LIMIT_4K | DA_32 | DPL1 | DA_DRW);

		pStack -= pJTab->stack;
	}

	proc_on_head = proc_on_view = pList;
	reenter = 0;

	memory_init();
	hd_init();
	buffer_init();
	keyboard_init();
	clock_init();
	proc_restart();

	for(; ;) { }
}

