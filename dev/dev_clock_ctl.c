#include "type.h"
#include "const.h"
#include "protect.h"
#include "clock.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "proto.h"


/*-----clock_init-----*/
void clock_init(void)
{
	out_byte(RATE_GENERATOR, TIMER_MODE, NO_DELAY);
	out_byte(TIMER0, (u8)(TIMER_FREQ / HZ), NO_DELAY);
	out_byte(TIMER0, (u8)((TIMER_FREQ / HZ) >> 8), NO_DELAY);

	kernel_ticks = 0;

	prot_set_ihandler(clock_handler, CLOCK_IRQ);
	enable_intr(CLOCK_IRQ);
}


/*-----clock_handler-----*/
void clock_handler(void)
{
	if(++kernel_ticks == 0xFFFFFFFF)
		kernel_ticks = 0;

	if(reenter > 0)
		return;

	if(proc_on_view->proc_ticks > 0)
		proc_on_view->proc_ticks--;

	proc_scheduling();
}
