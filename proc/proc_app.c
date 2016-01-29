#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"


/* local macro */
#define	ALL_T	(NR_ANT + NR_DF + NR_TEST)


/*-----proc_scheduling-----*/
inline void proc_scheduling(void)
{
	PROC	*pTab = &pList[0];
	int	highPrio = 0, nCir;

	for(nCir = 0; nCir < ALL_T; nCir++, pTab++) {
		if(pTab->proc_flags == 0) {
			if(pTab->proc_ticks > highPrio) {
				proc_on_view = pTab;
				highPrio = pTab->proc_ticks;
			}
		}
	}

	if(highPrio == 0) {
		for(; nCir > 0; nCir--, pTab--) {
			if(pTab->proc_ticks == 0)
				pTab->proc_ticks = pTab->proc_priority;
		}

		for(proc_on_view = NULL; nCir < ALL_T; nCir++, pTab++) {
			if(pTab->proc_flags == 0) {
				proc_on_view = pTab;
				break;
			}
		}

		if(proc_on_view == NULL) {
			sys_printx(MAGIC_KERN, "Keep your sword, go for fight", 29, 0);
		}
	}

	return;
}


/*-----proc_getpid-----*/
inline int proc_getpid(void)
{
	return	(proc_on_view->proc_id);
}


/*-----proc_cheating-----*/
inline void proc_cheating(u32 procOne, u32 procTwo)
{
	close_int();
	(proc_on_head + procOne)->proc_old_ticks = (proc_on_head + procOne)->proc_ticks;
	(proc_on_head + procTwo)->proc_old_ticks = (proc_on_head + procTwo)->proc_ticks;
	(proc_on_head + procTwo)->proc_ticks = (proc_on_head + procOne)->proc_ticks = CHEATING_TICKS;
	open_int();
}


/*-----proc_caught_cheat-----*/
inline void proc_caught_cheat(u32 procOne, u32 procTwo)
{
	u32	oTickOne = CHEATING_TICKS - pList[procOne].proc_ticks;
	u32	oTickTwo = CHEATING_TICKS - pList[procTwo].proc_ticks;
	
	pList[procOne].proc_ticks = ((oTickOne = (pList[procOne].proc_old_ticks - oTickOne)) > 0) ? oTickOne : 0;
	pList[procTwo].proc_ticks = ((oTickTwo = (pList[procTwo].proc_old_ticks - oTickTwo)) > 0) ? oTickTwo : 0;
}


/*-----proc_set_errno-----*/
inline void proc_set_errno(u32 procOff, int nError)
{
	close_int();
	(proc_on_head + procOff)->proc_errno = nError;
	open_int();
}
