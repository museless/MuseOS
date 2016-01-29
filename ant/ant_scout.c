#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "command.h"
#include "ant_job.h"
#include "proto.h"


static	PROC	*pScout = &pList[SCOUT];


/*-----scout_job-----*/
void scout_job(void)
{
	struct	message		scoutMsg;

	disp_str("Scout ant reported.\n");

	while(1) {
		block(SCOUT, NONPROC, BLOCK_ONE);

		memset(&scoutMsg, 0, sizeof(struct message));

		if(pScout->proc_state == ESTABLISH_PORT) {
			pList[DF_HD].proc_ticks = pList[DF_HD].proc_priority;
			scoutMsg.type = LISTEN;
			send_recv(SEND, DF_HD, &scoutMsg);
		} else {
			printx(MAGIC_KERN, "scout_ant_job: unknown stat %d", pScout->proc_state);
		}
	}
}
