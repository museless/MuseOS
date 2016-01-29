#include "type.h"
#include "const.h"
#include "proto.h"


/*-----vsprintf-----*/
int vsprintf(char *buf, char *fmt, va_list arg)
{
	va_list		pArg = arg;
	char		*pFmt, *pStr;
	int		sLen = 0;
	char		str[DEF_STRLEN];

	for(pFmt = fmt; *pFmt != '\0'; pFmt++) {
		if(*pFmt != '%') {
			*buf++ = *pFmt;
			continue;
		}

		switch(*++pFmt) {
		case '%':
			*buf++ = '%';
			break;

		case '0':
			*buf = '\0';
			return	sLen;
		
		case 'c':
			sLen++;
			*buf++ = *pArg;
			pArg += 4;
			break;

		case 'd':
			sLen++;
			itoa(*(u32 *)pArg, str, 10);
			strncpy(buf, str, strlen(str));
			buf += strlen(str);
			pArg += 4;
			break;

		case 'x':
			sLen++;
			*(str + 0) = '0';
			*(str + 1) = 'x';
			itoa(*(u32 *)pArg, str + 2, 16);
			strncpy(buf, str, strlen(str));
			buf += strlen(str);
			pArg += 4;
			break;

		case 's':
			sLen++;
			pStr = (char *)*(u32 *)pArg;
			strncpy(buf, pStr, strlen(pStr));
			buf += strlen(pStr);
			pArg += 4;
			break;

		default:
			*buf++ = ' ';
			break;
		}
	}

	*buf = 0;
	return	sLen;
}


/*-----itoa-----*/
void itoa(int figure, char *str, int base)
{
	int	pNum, sLen;
	
	if(figure == 0) {
		*str = 0x30;
		*(str + 1) = '\0';
		return;
	}

	for(sLen = 0, pNum = figure; pNum != 0; sLen++)
		pNum /= base;
	
	for(*(str + sLen--) = '\0'; sLen >= 0; sLen--) {
		pNum = figure;
		figure /= base;

		pNum = (pNum - figure * base);
		pNum += ((pNum < 10) ? 0x30 : (0x41 - 0xA));
		*(str + sLen) = pNum;
	}

	return;
}
