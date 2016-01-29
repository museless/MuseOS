#include "type.h"
#include "const.h"
#include "console.h"
#include "tty.h"
#include "keyboard.h"

/* global data */
u32	nr_current_console;

/* local function */
static	void	tty_do_read(struct ttybuf *pTty);
static	void	tty_do_write(struct ttybuf *pTty);

static	void	tty_put_key(u8 putCode, struct ttybuf *pTty);

/* local data */
struct	ttybuf	ttyTab[NR_TTY];


/*-----tty_init-----*/
void tty_init(void)
{
	struct	ttybuf	*pTty = ttyTab;

	for(; pTty < NR_TTY + ttyTab; pTty++) {
		pTty->t_tail = pTty->t_head = pTty->t_buf;
		pTty->t_bcount = 0;

		console_init(&pTty->t_console, pTty - ttyTab);
	}

	nr_current_console = 0;
}


/*-----tty_platform-----*/
void tty_platform(void)
{
	struct	ttybuf	*pTtab = ttyTab;

	tty_init();

	while(1) {
		for(pTtab = ttyTab; pTtab < ttyTab + NR_TTY; pTtab++) {
			tty_do_read(pTtab);
			tty_do_write(pTtab);
		}
	}
}


/*-----tty_process_key-----*/
void tty_process_key(u16 proCode, struct ttybuf *pTty)
{
	if(!(proCode & FLAG_EXT)) {
		tty_put_key((u8)proCode, pTty);

	} else {
		switch(proCode) {
		case ENTER:
			tty_put_key('\n', pTty);
			break;
		case BACKSPACE:
			tty_put_key('\b', pTty);
			break;
		case PAGEUP:
			console_scroll_screen(P_UP, PAGE, pTty);
			break;
		case PAGEDOWN:
			console_scroll_screen(P_DOWN, PAGE, pTty);
			break;
		case UP:
			console_scroll_screen(P_UP, LINE, pTty);
			break;
		case DOWN:
			console_scroll_screen(P_DOWN, LINE, pTty);
			break;
		case RIGHT:
			console_panning_cursor(C_RIGHT, pTty);
			break;
		case LEFT:
			console_panning_cursor(C_LEFT, pTty);
			break;
		case END:
			console_clean_screen(pTty);
			break;
		case F1:
			console_shift_screen(TTY_FIRST);
			break;
		case F2:
			console_shift_screen(TTY_SECOND);
			break;
		default:
			break;
		}
	}
}


/*-----tty_do_read-----*/
static void tty_do_read(struct ttybuf *pTty)
{
	if(is_current_console(pTty - ttyTab) == 0)
		keyboard_read_keybuf(pTty);
}


/*-----tty_put_key-----*/
static void tty_put_key(u8 putCode, struct ttybuf *pTty)
{
	if(pTty->t_bcount < MAX_TBUF) {
		if(pTty->t_head == pTty->t_buf + MAX_TBUF)
			pTty->t_head = pTty->t_buf;

		*pTty->t_head++ = putCode;
		pTty->t_bcount++;
	}
}


/*-----tty_do_write-----*/
static void tty_do_write(struct ttybuf *pTty)
{
	u8	out_code;

	if(pTty->t_bcount > 0) {
		if(pTty->t_tail == pTty->t_buf + MAX_TBUF)
			pTty->t_tail = pTty->t_buf;

		out_code = *pTty->t_tail++;
		pTty->t_bcount--;

		console_out_char(out_code, pTty);
	}
}
