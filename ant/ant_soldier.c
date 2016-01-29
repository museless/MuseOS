#include "type.h"
#include "const.h"
#include "protect.h"
#include "ant_job.h"
#include "proc.h"
#include "misc.h"
#include "command.h"
#include "proto.h"


static	PROC	*pSoldier = &pList[SOLDIER];


/*-----pSoldier_job-----*/
/* It looks like pSoldier carry a box the sender give him */
/* And when pSoldier reach the place and drop the box to  */
/* bring food or sth. Fin, it brings the box to a sender */
void soldier_job(void)
{
	struct	message		tranMsg;

	disp_str("Soldier ant reported.\n");

	while(1) {
		block(SOLDIER, NONPROC, BLOCK_ONE);
		proc_caught_cheat(SOLDIER, DF_HD);

		if(pSoldier->proc_state == GET_DATA || pSoldier->proc_state == TRAN_DATA ||
		   pSoldier->proc_state == GET_HDINFO) {
			memcpy(&tranMsg, pSoldier->proc_msg, sizeof(struct message));
			pSoldier->proc_msg = NULL;
			proc_cheating(SOLDIER, DF_HD);
			send_recv(SEND, DF_HD, &tranMsg);
		} else {
			printx(MAGIC_KERN, "pSoldier_job: unknown stat %d", pSoldier->proc_state);
		}

		pSoldier->proc_state = ALREADY_RUN;
	}
}
