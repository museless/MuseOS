/* the next data used in the kernel */
/* everything in the kernel can used */

#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "hd.h"
#include "console.h"
#include "tty.h"
#include "misc.h"
#include "proto.h"

u32	disp_pos;
u32	kernel_ticks;

JOB	jList[NR_ANT + NR_DF + NR_TEST] = {	{"QUEEN", QUEEN_PLC, queen_job},
						{"SOLDIER", SOLDIER_PLC, soldier_job},
						{"SCOUT", SCOUT_PLC, scout_job},
						{"MID_WORKER", MID_WORKER_PLC, mid_worker_job},
						{"SMALL_WORKER", SMALL_WORKER_PLC, small_worker_job},
						{"NURSERY_ANT", NURSERY_ANT_PLC, nursery_job},
						{"DF_TTY", DF_TTY_PLC, tty_platform},
						{"DF_HD", DF_HD_PLC, hd_platform},
						{"DF_MUSEFS", DF_MFS_PLC, musefs_platform},

						{"TEST", TEST_PLC, test}};


sys_call	sysTab[NR_SYS] = {sys_sendrec, sys_printx, sys_block};
