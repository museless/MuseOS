#include "type.h"
#include "ctype.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"
#include "musefs.h"
#include "command.h"
#include "proto.h"


/*-----musefs_cross_space-----*/
char *musefs_cross_space(char *pBuf, int bLimit)
{
	for(; bLimit > 0 && isblank(*pBuf); bLimit--, pBuf++)
		;	/* nothing */

	return	(isdight(*pBuf) || bLimit == 0) ? NULL : pBuf;
}


/*-----musefs_strip_path-----*/
char *musefs_strip_path(char *fPath, u32 *pLen)
{
	u32	strLen;

	for(strLen = 0; !ispath(*fPath) && *fPath != 0; strLen++, fPath++)
		;	/* nothing */

	*pLen = strLen;
	return	((*fPath) ? fPath : NULL);
}


/*-----musefs_get_dst-----*/
char *musefs_get_dst(char *pName)
{
	int	sLen = strlen(pName);
	
	while(sLen-- > 0) {
		if(ispath(pName[sLen]))
			break;
	}

	return	(pName + sLen + 1);
}


/*-----musefs_block_to_lba-----*/
u32 musefs_block_to_lba(u32 nBlock)
{
	return	(nBlock * (sbList[0].psb_stru.sb_block_size >> PER_SSIZE_SFT));
}
