#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"


/* local function */
static	int	sr_msg_send(int rDest, int sSrc, struct message *sendMsg);
static	int	sr_msg_recv(int nRecver, int nSender, struct message *recvMsg);


/*-----sys_sendrec-----*/
int sys_sendrec(int funType, int destOff, struct message *sendMsg, PROC *srcProc)
{
	int	fRet, reqSrc = srcProc->proc_id;

	if(funType == RECEIVE)
		memset(sendMsg, 0, sizeof(struct message));

	switch(funType) {
	case SEND:
		fRet = sr_msg_send(destOff, reqSrc, sendMsg);
		break;
	case RECEIVE:
		fRet = sr_msg_recv(reqSrc, destOff, sendMsg);
		break;
	case BOTH:
		if((fRet = sr_msg_send(destOff, reqSrc, sendMsg)) == 0)
			sr_msg_recv(reqSrc, destOff, sendMsg);
		break;
	default:
		assert((funType == SEND) || (funType == RECEIVE));
	}

	return	fRet;
}


/*-----sr_msg_send-----*/
static int sr_msg_send(int rDest, int sSrc, struct message *sendMsg)
{
	PROC	*pRecv = pList + rDest;
	PROC	*pSend = pList + sSrc;

	assert(sSrc != rDest);

	if(pRecv->proc_flags == RECEVING && (pRecv->proc_recvfrom == sSrc || pRecv->proc_recvfrom == ANY))  {
		assert(sendMsg);

		memcpy(prot_vatoma(rDest, pRecv->proc_msg), prot_vatoma(sSrc, sendMsg), sizeof(struct message));

		pRecv->proc_recvfrom = NO_TASK;
		pRecv->proc_flags &= ~RECEVING;
		pRecv->proc_msg = 0;

		assert(pSend->proc_sendto == NO_TASK);
		assert(pSend->proc_recvfrom == NO_TASK);
		assert(pSend->proc_msg == 0);
		assert(!(pSend->proc_flags & ~SENDING));
		assert(pRecv->proc_sendto == NO_TASK);
		assert(pRecv->proc_recvfrom == NO_TASK);
		assert(pRecv->proc_msg == 0);
		assert(!(pRecv->proc_flags & ~RECEVING));

	} else {
		pSend->proc_flags |= SENDING;
		pSend->proc_msg = sendMsg;
		pSend->proc_sendto = rDest;

		if(pRecv->proc_send_next == NULL) {
			pRecv->proc_send_next = pSend;
		} else {
			PROC	*rTmp = pRecv;

			while(rTmp->proc_send_next != NULL)
				rTmp = rTmp->proc_send_next;

			rTmp->proc_send_next = pSend;
		}

		pSend->proc_send_next = NULL;

		proc_scheduling();
		assert(pSend->proc_msg != 0);
		assert(pSend->proc_flags & SENDING);
		assert(pSend->proc_sendto != NO_TASK);
		assert(pSend->proc_recvfrom == NO_TASK);
	}

	return	0;
}


/*-----sr_msg_recv-----*/
static int sr_msg_recv(int nRecver, int nSender, struct message *recvMsg)
{
	PROC	*pSend = pList + nSender;
	PROC	*pRecv = pList + nRecver;
	PROC	*pList;
	int	cpyFlags = 0;

	assert(nRecver != nSender);

	if(nSender == ANY) {
		cpyFlags = 1;
		pSend = pRecv->proc_send_next;
		pRecv->proc_send_next = pSend->proc_send_next;
		pSend->proc_send_next = NULL;

		assert(pRecv->proc_msg == NULL);
		assert(!(pRecv->proc_flags & RECEVING));
		assert(pRecv->proc_sendto == NO_TASK);
		assert(pRecv->proc_recvfrom == NO_TASK);

	} else {
		if(pSend->proc_flags & SENDING && pSend->proc_sendto == nRecver) {
			cpyFlags = 1;

			assert(pRecv->proc_send_next);

			for(pList = pRecv; pList != NULL; pList = pList->proc_send_next) {
				if(pList->proc_send_next == pSend) {
					pList->proc_send_next = pSend->proc_send_next;
					break;
				}
			}
	
			assert(pRecv->proc_msg == NULL);
			assert(!(pRecv->proc_flags & RECEVING));
			assert(pRecv->proc_sendto == NO_TASK);
			assert(pRecv->proc_recvfrom == NO_TASK);
			assert(pSend->proc_msg != NULL);
			assert(pSend->proc_flags & SENDING);
			assert(pSend->proc_sendto == nRecver);
			assert(pSend->proc_recvfrom == NO_TASK);
		}
	}

	if(cpyFlags) {
		assert(recvMsg);
		memcpy(prot_vatoma(nRecver, recvMsg), prot_vatoma(nSender, pSend->proc_msg), sizeof(struct message));

		pSend->proc_msg = 0;
		pSend->proc_flags &= ~SENDING;
		pSend->proc_sendto = NO_TASK;

	} else {
		pRecv->proc_flags |= RECEVING;

		pRecv->proc_recvfrom = pSend->proc_id;
		pRecv->proc_msg = recvMsg;

		proc_scheduling();

		assert(pRecv->proc_flags & RECEVING);
		assert(pRecv->proc_msg != NULL);
		assert(pRecv->proc_sendto == NO_TASK);
		assert(pRecv->proc_recvfrom != NO_TASK);
	}

	return	0;
}



