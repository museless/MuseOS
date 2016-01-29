#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "musefs.h"
#include "command.h"
#include "proto.h"


/*-----test-----*/
void test(void)
{	
	struct	message	tranMsg;

	/* for hd */
	/*
	u8	rBuf[512];

	tranMsg.type = READ;
	tranMsg.source = TEST;
	tranMsg.DRIVE = 0;
	tranMsg.LBA = 0;
	tranMsg.SIZE = 512;
	tranMsg.BUF = rBuf;

	hd_access_ctl(GET_DATA, &tranMsg);
	
	int	nCir;

	for(nCir = 0; nCir < 512; nCir++)
		printx(MAGIC_USR, "%x ", rBuf[nCir]);*/

	tranMsg.source = TEST;
	tranMsg.type = FS_CREAT;
	tranMsg.PATHNAME = "/fuck";

	musefs_access_ctl(FS_RDWR, &tranMsg);
	printx(MAGIC_USR, "%d\n", pList[TEST].proc_errno);

	stop();
}

