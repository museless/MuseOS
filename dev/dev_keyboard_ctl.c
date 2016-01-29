#include "type.h"
#include "const.h"
#include "protect.h"
#include "console.h"
#include "tty.h"
#include "keyboard.h"
#include "keymap.h"
#include "proto.h"


/* local function */
static	u8	keyboard_get_byte(void);

/* local data */
static	KB_BUF	kb_buf;

u8	l_shift, r_shift;
u8	l_alt, r_alt;
u8	l_ctl, r_ctl;
u8	caps_lock, num_lock, scr_lock;


/*-----keyboard_init-----*/
void keyboard_init(void)
{
	num_lock = scr_lock = caps_lock = 0;
	l_shift = r_shift = 0;
	l_alt = r_alt = 0;
	l_ctl = r_ctl = 0;

	kb_buf.tail = kb_buf.head = kb_buf.buf;
	kb_buf.count = 0;

	prot_set_ihandler(keyboard_handler, KEYBOARD_IRQ);
	enable_intr(KEYBOARD_IRQ);
}


/*-----keyboard_handler-----*/
void keyboard_handler(void)
{
	u8	scan_code = in_byte(KB_DATA);

	if(kb_buf.count < MAX_KBBUF) {
		if(kb_buf.head == kb_buf.buf + MAX_KBBUF)
			kb_buf.head = kb_buf.buf;
	
		*kb_buf.head++ = scan_code;
		kb_buf.count++;
	}
}


/*-----keyboard_read_keybuf-----*/
void keyboard_read_keybuf(struct ttybuf *tty)
{
	if(kb_buf.count > 0) {
		u16	code;
		u8	i, make, code_with_E0 = 0;

		code = keyboard_get_byte();

		if(code == 0xE1) {
			u8	pausebrk_code[6] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
			u8	is_pausebrk = 1;

			for(i = 1; i < 6; i++) {
				if(pausebrk_code[i] != keyboard_get_byte()) {
					is_pausebrk = 0;
					break;
				}
			}

			if(is_pausebrk)
				code = PAUSE_BREAK;

		} else if(code == 0xE0) {
			code = (u16)keyboard_get_byte();

			if(code == 0x2A) {
				if(keyboard_get_byte() == 0xE0)
					if(keyboard_get_byte() == 0x3A) {
						code = PAUSE_BREAK;
						make = 1;
					}

			} else if(code == 0xB7) {
				if(keyboard_get_byte() == 0xE0)
					if(keyboard_get_byte() == 0xAA)
						make = 0;
			} else {
				code_with_E0 = 1;
			}
		} 
		
		if(code != PAUSE_BREAK && code != PRINT_SCREEN) {
			u16	*keyrow, column = 0, caps = l_shift || r_shift;

			make = (code & FLAG_BREAK ? 0 : 1);
			keyrow = &keymap[code * NR_LINE];

			if(caps_lock) {
				if(keyrow[0] >= 'a' && keyrow[0] <= 'z')
					caps = !caps;
			}

			if(caps)
				column = 1;

			if(code_with_E0)
				column = 2;

			code = keyrow[column];

			if((!(code & FLAG_EXT)) && make) {
				tty_process_key(code, tty);
				return;
			}

			switch(code) {
			case L_CTL:
				l_ctl = make;
				break;
			case R_CTL:
				r_ctl = make;
				break;
			case L_SHIFT:
				l_shift = make;
				break;
			case R_SHIFT:
				r_shift = make;
				break;
			case L_ALT:
				l_alt = make;
				break;
			case R_ALT:
				r_alt = make;
				break;
			case CAPS_LOCK:
				if(make) {
					caps_lock = !caps_lock;
				}
				break;
			case SCROLL_LOCK:
				if(make) {
					scr_lock = !scr_lock;
				}
				break;
			case NUM_LOCK:
				if(make) {
					num_lock = !num_lock;
				}
				break;
			default:
				break;
			}

			if(make)
				tty_process_key(code, tty);
		}
	}
}


/*-----keyboard_get_byte-----*/
static u8 keyboard_get_byte(void)
{
	u8	byte;

	while(1) {
		if(kb_buf.count > 0) {
			if(kb_buf.tail == kb_buf.buf + MAX_KBBUF)
				kb_buf.tail = kb_buf.buf;
		
			byte = *kb_buf.tail++;	
			kb_buf.count--;
	
			return	byte;
		}
	}
}
