#ifndef	_MUSE_KEYBOARD_H_
#define	_MUSE_KEYBOARD_H_

#define	NR_SCANCODE	0x80
#define	NR_LINE		0x3

#define	MAX_KBBUF	0x40

#define	FLAG_EXT	0x100
#define	FLAG_BREAK	0x80

#define	ESC		(FLAG_EXT + 0x1)
#define	BACKSPACE	(FLAG_EXT + 0x2)
#define	TAB		(FLAG_EXT + 0x3)
#define	ENTER		(FLAG_EXT + 0x4)
#define	L_CTL		(FLAG_EXT + 0x5)
#define	R_CTL		(FLAG_EXT + 0x6)
#define	L_SHIFT		(FLAG_EXT + 0x7)
#define	R_SHIFT		(FLAG_EXT + 0x8)
#define	R_ALT		(FLAG_EXT + 0x9)
#define	L_ALT		(FLAG_EXT + 0xA)
#define	CAPS_LOCK	(FLAG_EXT + 0xB)
#define	F1		(FLAG_EXT + 0xC)
#define	F2		(FLAG_EXT + 0xD)
#define	F3		(FLAG_EXT + 0xE)
#define	F4		(FLAG_EXT + 0xF)
#define	F5		(FLAG_EXT + 0x10)
#define	F6		(FLAG_EXT + 0x11)
#define	F7		(FLAG_EXT + 0x12)
#define	F8		(FLAG_EXT + 0x13)
#define	F9		(FLAG_EXT + 0x14)
#define	F10		(FLAG_EXT + 0x15)
#define	F11		(FLAG_EXT + 0x16)
#define	F12		(FLAG_EXT + 0x17)
#define	NUM_LOCK	(FLAG_EXT + 0x18)
#define	SCROLL_LOCK	(FLAG_EXT + 0x19)
#define	INSERT		(FLAG_EXT + 0x1A)
#define	DELETE		(FLAG_EXT + 0x1B)
#define	PRINT_SCREEN	(FLAG_EXT + 0x1C)
#define	PAUSE_BREAK	(FLAG_EXT + 0x1D)
#define	PAGEUP		(FLAG_EXT + 0x1E)
#define	PAGEDOWN	(FLAG_EXT + 0x1F)
#define	END		(FLAG_EXT + 0x20)
#define	UP		(FLAG_EXT + 0x21)
#define	DOWN		(FLAG_EXT + 0x22)
#define	RIGHT		(FLAG_EXT + 0x23)
#define	LEFT		(FLAG_EXT + 0x24)
#define	HOME		(FLAG_EXT + 0x25)

typedef	struct	keyboard_buf {
	u8	*head;
	u8	*tail;
	u8	buf[MAX_KBBUF];
	int	count;
} KB_BUF;

/* function prototype */
void	keyboard_handler(void);
void	keyboard_read_keybuf(struct ttybuf *tty);

#endif

