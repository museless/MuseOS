#ifndef	_MUSE_CONST_H_
#define	_MUSE_CONST_H_

#include "errno.h"

#define	ASSERT
#ifdef	ASSERT
void	assertion_failure(char *exp, char *file, char *base_file, int line);
#define	assert(exp)	if(exp) ; \
			else	assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
#define	assert(exp)
#endif

#define	NULL		(void *)0
#define	FAILURE		-1

/* used in init protect mode */
#define	NR_GDT		256
#define	NR_IDT		256
#define	NR_LDT		3

#define	NR_SYS		3
	
#define	NR_INTR		16

/* mask */
#define	U32_MASK	0xFFFFFFFF

/* str & buf */
#define	DEF_STRLEN	256
#define	DEF_NAMELEN	16

#define	MAXBUFSIZE	4096

/* ticks in the service ring thing */
#define	CHEATING_TICKS	0x22
#define	DEFAULT_TICKS	0x20

/* color */
#define	V_MEM_BASE	0xB8000
#define	V_MEM_SIZE	0x7FFF
#define	ONE_PAGE_SIZE	((80 * 25) << 1)
#define	ONE_LINE_SIZE	(80 << 1)

#define	BLACK		0x0
#define	D_BLUE		0x1
#define	GREEN		0x2
#define	L_BLUE		0x3
#define	RED		0x4
#define	L_RED		0xC
#define	PURPLE		0x5
#define	COFFEE		0x6
#define	GRAY		0x7
#define	LIGHT		0x8

#define	DEF_MSG_COLOR	((LIGHT | RED) | D_BLUE << 4)

#define	TAB_OFFSET	0x4

/* i8259A */
#define	NR_IRQ		16
#define	CLOCK_IRQ 	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2
#define	ETHER_IRQ	3	
#define	SECONDARY_IRQ	3	
#define	RS232_IRQ	4	
#define	XT_WINI_IRQ	5	
#define	FLOPPY_IRQ	6	
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14

/* proc */
#define	NR_NAME_LEN	32

/* used in the port to sign i/o delay */
#define	DELAY		1
#define	NO_DELAY	0

/* magic char used in printx */
#define	MAGIC_KERN	'\111'
#define	MAGIC_USR	'\112'

/* tid stand */
#define	NO_TASK		-1
#define	ANY		200
#define	QUEEN		0
#define	SOLDIER		1
#define	SCOUT		2
#define	MID_WORKER	3
#define	SMALL_WORKER	4
#define	NURSERY_ANT	5

#define	DF_TTY		6
#define	DF_HD		7
#define	DF_MFS		8

#define	TEST		9

/* three syscall */
#define	SEND		0x1
#define	RECEIVE		0x2
#define	BOTH		0x4

/* the use of message stru */
#define	SIZE		m.d1.a1
#define	LBA		m.d1.a2
#define	DRIVE		m.d1.a3
#define	DEVICE		m.d1.a4
#define	BUF		m.d1.a5
#define	PATHNAME	m.d1.a6

/* inform call's lock & msg */
#define	NONLOCK		0
#define	LOCK		1

/* kb control */
#define	KB_CMD		0x64
#define	KB_DATA		0x60

/* VGA */
#define	CRTC_ADDR_REG	0x3D4
#define	CRTC_DATA_REG	0x3D5
#define	START_ADDR_H	0xC
#define	START_ADDR_L	0xD
#define	CURSOR_H	0xE
#define	CURSOR_L	0xF

#endif
