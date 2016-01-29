#ifndef	_MUSE_TTY_H_
#define	_MUSE_TTY_H_

/* macro */
#define	MAX_TBUF	0x40
#define	NR_TTY		0x2

#define	TTY_FIRST	0x0
#define	TTY_SECOND	0x1

struct	ttybuf {
	u8		*t_head;
	u8		*t_tail;
	u8		t_buf[MAX_TBUF];
	int		t_bcount;

	struct	video	t_console;
};

/* global data */
extern	u32		nr_current_console;
extern	struct	ttybuf	ttyTab[NR_TTY];

/* function prototype */
/* file_tty_ctl.c */
void	tty_platform(void);
void	tty_process_key(u16 proCode, struct ttybuf *pTty);

/* file_console_ctl.c */
void	console_out_char(u8 pChar, struct ttybuf *pTty);
void	console_out_string(char *pStr);

void	console_scroll_screen(int sToward, int sType, struct ttybuf *pTty);
void	console_panning_cursor(int pType, struct ttybuf *pTty);
void	console_adjust_screen(struct ttybuf *pTty);
void	console_clean_screen(struct ttybuf *pTty);
void	console_shift_screen(int nr_console);

#endif
