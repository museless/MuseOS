#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"
#include "nest.h"
#include "proto.h"


/* local data */
static	FDBLK	*p_FHash = (FDBLK *)(BUFFER_HL_OFF);
static	FDBLK	*p_FList = (FDBLK *)(BUFFER_LIST_OFF);
static	char	*p_FAddr = (char *)(BUFFER_RL_OFF);


/*-----buffer_init-----*/
void buffer_init(void)
{
	FDBLK	fillBlk, *p_LHead = p_FList + 1;
	char	*p_FHead = p_FAddr + PER_BUFFER_BYTE;
	int	cirVal;

	for(cirVal = 1; cirVal < NR_BUFFER_BLK; cirVal++, p_FHead += PER_BUFFER_BYTE, p_LHead++) {
		fillBlk.p_list_head = p_LHead - 1;
		fillBlk.p_list_next = p_LHead + 1;

		fillBlk.data = p_FHead;
		memcpy(p_LHead, &fillBlk, BUFFER_STRU_LEN);
	}

	fillBlk.p_list_head = --p_LHead;
	fillBlk.p_list_next = p_FList + 1;
	fillBlk.data = p_FAddr;

	memcpy(p_FList, &fillBlk, BUFFER_STRU_LEN);

	p_LHead->p_list_next = p_FList;
}
