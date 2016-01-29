#ifndef	_MUSE_KEYMAP_H_
#define	_MUSE_KEYMAP_H_

/* keymap for asus-x45v	*/

u16	keymap[NR_SCANCODE * NR_LINE] = {
/* scan_code		!shift		shift		E0 */
/* 0x0 - none	*/	0,		0,		0,
/* 0x1 - ESC	*/	ESC,		ESC,		0,
/* 0x2 - 1	*/	'1',		'!',		0,
/* 0x3 - 2	*/	'2',		'@',		0,
/* 0x4 - 3	*/	'3',		'#',		0,
/* 0x5 - 4	*/	'4',		'$',		0,
/* 0x6 - 5	*/	'5',		'%',		0,
/* 0x7 - 6	*/	'6',		'^',		0,
/* 0x8 - 7	*/	'7',		'&',		0,
/* 0x9 - 8	*/	'8',		'*',		0,
/* 0xA - 9	*/	'9',		'(',		0,
/* 0xB - 0	*/	'0',		')',		0,
/* 0xC - -	*/	'-',		'_',		0,
/* 0xD - =	*/	'=',		'+',		0,
/* 0xE - BACKSP	*/	BACKSPACE,	BACKSPACE,	0,
/* 0xF - TAB	*/	TAB,		TAB,		0,
/* 0x10 - q	*/	'q',		'Q',		0,
/* 0x11 - w	*/	'w',		'W',		0,
/* 0x12 - e	*/	'e',		'E',		0,
/* 0x13 - r	*/	'r',		'R',		0,
/* 0x14 - t	*/	't',		'T',		0,
/* 0x15 - y	*/	'y',		'Y',		0,
/* 0x16	- u	*/	'u',		'U',		0,
/* 0x17 - i	*/	'i',		'I',		0,
/* 0x18	- o	*/	'o',		'O',		0,
/* 0x19 - p	*/	'p',		'P',		0,
/* 0x1A	- [	*/	'[',		'{',		0,
/* 0x1B	- ]	*/	']',		'}',		0,
/* 0x1C - ENTER	*/	ENTER,		ENTER,		0,
/* 0x1D - L_CTL	*/	L_CTL,		L_CTL,		R_CTL,
/* 0x1E - a	*/	'a',		'A',		0,
/* 0x1F	- s	*/	's',		'S',		0,
/* 0x20 - d	*/	'd',		'D',		0,
/* 0x21 - f	*/	'f',		'F',		0,
/* 0x22	- g	*/	'g',		'G',		0,
/* 0x23	- h	*/	'h',		'H',		0,
/* 0x24	- j	*/	'j',		'J',		0,
/* 0x25 - k	*/	'k',		'K',		0,
/* 0x26	- l	*/	'l',		'L',		0,
/* 0x27 - ;	*/	';',		':',		0,
/* 0x28 - '	*/	'\'',		'"',		0,
/* 0x29 - `	*/	'`',		'~',		0,
/* 0x2A - L_SHF	*/	L_SHIFT,	L_SHIFT,	0,
/* 0x2B - \	*/	'\\',		'|',		0,
/* 0x2C - z	*/	'z',		'Z',		0,
/* 0x2D - x	*/	'x',		'X',		0,
/* 0x2E - c	*/	'c',		'C',		0,
/* 0x2F	- v	*/	'v',		'V',		0,
/* 0x30 - b	*/	'b',		'B',		0,
/* 0x31	- n	*/	'n',		'N',		0,
/* 0x32	- m	*/	'm',		'M',		0,
/* 0x33	- ,	*/	',',		'<',		0,
/* 0x34 - .	*/	'.',		'>',		0,
/* 0x35 - /	*/	'/',		'?',		0,
/* 0x36 - R_SHF	*/	R_SHIFT,	R_SHIFT,	0,
/* 0x37 - TIMES	*/	'*',		0,		0,
/* 0x38	- R_ALT	*/	R_ALT,		R_ALT,		L_ALT,
/* 0x39	- SPACE	*/	' ',		' ',		0,
/* 0x3A	- CAPS	*/	CAPS_LOCK,	CAPS_LOCK,	0,
/* 0x3B - F1	*/	F1,		F1,		0,
/* 0x3C - F2	*/	F2,		F2,		0,
/* 0x3D	- F3	*/	F3,		F3,		0,
/* 0x3E	- F4	*/	F4,		F4,		0,
/* 0x3F	- F5	*/	F5,		F5,		0,
/* 0X40 - F6	*/	F6,		F6,		0,
/* 0x41 - F7	*/	F7,		F7,		0,
/* 0x42	- F8	*/	F8,		F8,		0,
/* 0x43 - F9	*/	F9,		F9,		0,
/* 0x44 - F10	*/	F10,		F10,		0,
/* 0x45 - NUM_L	*/	NUM_LOCK,	NUM_LOCK,	0,
/* 0X46 - SCR_LK */	SCROLL_LOCK,	SCROLL_LOCK,	0,
/* 0x47 - PAD_7 */	'7',		0,		HOME,
/* 0x48 - PAD_8 */	'8',		0,		UP,
/* 0x49 - PAD_9 */	'9',		0,		PAGEUP,
/* 0x4A - PAD_-	*/	'-',		0,		0,
/* 0x4B - PAD_4	*/	'4',		0,		RIGHT,
/* 0x4C - PAD_5	*/	'5',		0,		0,
/* 0x4D	- PAD_6	*/	'6',		0,		LEFT,
/* 0x4E - PAD_DOT */	'.',		0,		0,
/* 0x4F	- PAD_1 */	'1',		0,		END,
/* 0x50 - PAD_2 */	'2',		0,		DOWN,
/* 0x51 - PAD_3	*/	'3',		0,		PAGEDOWN,
/* 0x52 - INS	*/	0,		0,		INSERT,
/* 0x53 - DEL	*/	0,		0,		DELETE,
/* 0x54 - none	*/	0,		0,		0,
/* 0x55 - none	*/	0,		0,		0,
/* 0x56 - none	*/	0,		0,		0,
/* 0x57 - none	*/	0,		0,		0,
/* 0x58 - none	*/	0,		0,		0,
/* 0x59 - none	*/	0,		0,		0,
/* 0x5A - none	*/	0,		0,		0,
/* 0x5B - none	*/	0,		0,		0,
/* 0x5C - none	*/	0,		0,		0,
/* 0x5D - none	*/	0,		0,		0,
/* 0x5E - none	*/	0,		0,		0,
/* 0x5F - none	*/	0,		0,		0,
/* 0x60 - none	*/	0,		0,		0,
/* 0x61 - none	*/	0,		0,		0,
/* 0x62 - none	*/	0,		0,		0,
/* 0x63 - none	*/	0,		0,		0,
/* 0x64 - none	*/	0,		0,		0,
/* 0x65 - none	*/	0,		0,		0,
/* 0x66 - none	*/	0,		0,		0,
/* 0x67 - none	*/	0,		0,		0,
/* 0x68 - none	*/	0,		0,		0,
/* 0x69 - none	*/	0,		0,		0,
/* 0x6A - none	*/	0,		0,		0,
/* 0x6B - none	*/	0,		0,		0,
/* 0x6C - none	*/	0,		0,		0,
/* 0x6D - none	*/	0,		0,		0,
/* 0x6E - none	*/	0,		0,		0,
/* 0x6F - none	*/	0,		0,		0,
/* 0x70 - none	*/	0,		0,		0,
/* 0x71 - none	*/	0,		0,		0,
/* 0x72 - none	*/	0,		0,		0,
/* 0x73 - none	*/	0,		0,		0,
/* 0x74 - none	*/	0,		0,		0,
/* 0x75 - none	*/	0,		0,		0,
/* 0x76 - none	*/	0,		0,		0,
/* 0x77 - none	*/	0,		0,		0,
/* 0x78 - none	*/	0,		0,		0,
/* 0x79 - none	*/	0,		0,		0,
/* 0x7A - none	*/	0,		0,		0,
/* 0x7B - none	*/	0,		0,		0,
/* 0x7C - none	*/	0,		0,		0,
/* 0x7D - none	*/	0,		0,		0,
/* 0x7E - none	*/	0,		0,		0,
/* 0x7F - none	*/	0,		0,		0,
};

#endif
