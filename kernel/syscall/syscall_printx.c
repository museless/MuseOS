#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "console.h"
#include "tty.h"
#include "proto.h"


/*-----printx-----*/
int printx(char magic, char *fmt, ...)
{
	va_list		arg = (va_list)((char *)&fmt + 4);
	int		len;
	char		buf[DEF_STRLEN];

	vsprintf(buf, fmt, arg);
	len = strlen(buf);

	syscall_print(magic, buf, len);

	return	0;
}


/*-----sys_printx-----*/
int sys_printx(char magic, char *buf, int len, int unused)
{
	u8	*pVmem = (u8 *)V_MEM_BASE;

	if(magic == MAGIC_KERN) {
		pVmem += 20 * 80;

		for(; len > 0; len--) {
			*pVmem++ = *buf++;
			*pVmem++ = (RED | GRAY << 4);
		}

		__asm__ __volatile__("cli");
		__asm__ __volatile__("hlt");

	} else if(magic == MAGIC_USR)
		console_out_string(buf);

	return	0;
}
