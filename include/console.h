#ifndef	_MUSE_CONSOLE_H_
#define	_MUSE_CONSOLE_H_

struct	video {
	u32	con_orig_addr;
	u32	con_cur_staddr;
	u32	con_cursor;
	u32	con_vlimit;
};

/* macro data */
#define	P_UP		0x0
#define	P_DOWN		0x1

#define	C_RIGHT		0x0
#define	C_LEFT		0x1

#define	PAGE		0x0
#define	LINE		0x1

#define	THE_LAST_PAGE	0x2EE0
#define	THE_LAST_LINE	0x3E80

/* function prototype */
/* console.c */
void	console_init(struct video *pCon, int nConsole);

inline	int	is_current_console(int nConsole);

#endif

