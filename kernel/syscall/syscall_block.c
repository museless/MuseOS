#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "proto.h"


/*-----unblock-----*/
/* used in ring0 */
void unblock(int procOff)
{
	pList[procOff].proc_flags &= UNBLOCK_MASK;
}


/*-----sys_block-----*/
void sys_block(int procOne, int procTwo, int blockType, PROC *srcProc)
{
	switch(blockType) {
	  case BLOCK_ONE:	pList[procOne].proc_flags |= BLOCK;		break;
	  case UNBLOCK_TWO:	pList[procTwo].proc_flags &= UNBLOCK_MASK;	break;
	  case BLK_ONE_UN_TWO:	pList[procOne].proc_flags |= BLOCK;
	  			pList[procTwo].proc_flags &= UNBLOCK_MASK;	break;
	  default:		printx(MAGIC_KERN, "Unknown blocktype: %x\n", blockType);
	}
	
	proc_scheduling();
}
