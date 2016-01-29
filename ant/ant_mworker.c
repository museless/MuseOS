#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "musefs.h"
#include "proto.h"


static	PROC	*pMworker = &pList[MID_WORKER];


/*-----mid_worker_job-----*/
void mid_worker_job(void)
{
	struct	message		tranMsg;

	disp_str("Middle worker ant reported.\n");

	while(1) {
		block(MID_WORKER, NONPROC, BLOCK_ONE);
		proc_caught_cheat(MID_WORKER, DF_MFS);

		proc_cheating(MID_WORKER, DF_MFS);
		if((pMworker->proc_msg->type & FS_STMASK) && pMworker->proc_msg->PATHNAME) {
			memcpy(&tranMsg, pMworker->proc_msg, sizeof(struct message));
			pMworker->proc_msg = NULL;
			send_recv(SEND, DF_MFS, &tranMsg);
		} else {
			pList[pMworker->proc_msg->source].proc_errno = (!pMworker->proc_msg->PATHNAME) ? EPATH : EPSTATE;
			unblock(pMworker->proc_msg->source);
		}

		pMworker->proc_state = ALREADY_RUN;
	}
}
