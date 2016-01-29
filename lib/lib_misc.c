#include "type.h"
#include "const.h"
#include "proto.h"


/*-----assertion_failure-----*/
void assertion_failure(char *eName, char *fName, char *baseFile, int nLine)
{
	printx(MAGIC_KERN, "assert(%s), file: %s, base_file: %s, line: %d", eName, fName, baseFile, nLine);
}


/*-----send_recv-----*/
int send_recv(int funType, int sDest, struct message *tranMsg)
{
	int	fRet;

	switch(funType) {
	case SEND:
	case RECEIVE:
		fRet = syscall_sendrecv(funType, sDest, tranMsg);
		break;
	default:
		assert((funType == SEND) || (funType == RECEIVE));
	}

	return	fRet;
}


/*-----block-----*/
int block(int procOne, int procTwo, int blockType)
{
	syscall_block(procOne, procTwo, blockType);
	return	0;
}
