#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "console.h"
#include "tty.h"
#include "misc.h"
#include "proto.h"


/* local define */
#define	LCONSOLE_HEAD	0x4

/* local function */
static	void	console_set_staddr(u32 sAddr);
static	void	console_set_cursor(u32 cPos);
static	void	console_set_head(int nConsole);

static	void	console_reflash(struct ttybuf *pTty);


/*-----console_init-----*/
void console_init(struct video *pCon, int nConsole)
{
	pCon->con_orig_addr = pCon->con_cur_staddr = (nConsole * THE_LAST_LINE);
	pCon->con_vlimit = THE_LAST_LINE;

	if(nConsole == 0) {
		pCon->con_cursor = disp_pos;
		disp_pos = 0;

	} else {
		pCon->con_cursor = pCon->con_orig_addr;
		console_set_head(nConsole);
	}

	console_set_cursor(pCon->con_cursor);
}


/*-----console_out_string-----*/
void console_out_string(char *pStr)
{
	struct	ttybuf	*pTty = &ttyTab[proc_on_view->proc_ntty];

	while(*pStr)
		console_out_char(*pStr++, pTty);
}


/*-----console_out_char-----*/
void console_out_char(u8 pChar, struct ttybuf *pTty)
{
	struct	video	*pCon = &pTty->t_console;
	u8		*pVmem = (u8 *)(V_MEM_BASE + pCon->con_cursor);
	int		cLimit = pCon->con_orig_addr + THE_LAST_LINE;

	switch(pChar) {
	case '\n':
		if(pCon->con_cursor < cLimit - ONE_LINE_SIZE)
			pCon->con_cursor = (pCon->con_cursor / ONE_LINE_SIZE + 1) * ONE_LINE_SIZE;

		if(pCon->con_cursor - pCon->con_cur_staddr == ONE_PAGE_SIZE - ONE_LINE_SIZE)
			pCon->con_cur_staddr += ONE_LINE_SIZE;

		break;
	case '\b':
		if(pCon->con_cursor > pCon->con_orig_addr) {
			*(pVmem - 1) = LIGHT;
			*(pVmem - 2) = ' ';
			pCon->con_cursor -= 2;
		} break;
	case '\t':
		if(pCon->con_cursor < cLimit - TAB_OFFSET)
			pCon->con_cursor += (TAB_OFFSET << 1);

		break;
	default:
		if(pCon->con_cursor >= pCon->con_orig_addr && pCon->con_cursor < cLimit) {
			*pVmem++ = pChar;
			*pVmem++ = GREEN;
			pCon->con_cursor += 2;
		} break;
	}

	if(pCon->con_cursor > cLimit - ONE_LINE_SIZE && pCon->con_cursor < cLimit)
		console_adjust_screen(pTty);

	console_reflash(pTty);
}


/*-----console_adjust_screen-----*/
void console_adjust_screen(struct ttybuf *pTty)
{
	u8	vBuf[ONE_LINE_SIZE];
	u8	*pSrc, *pDst;
	u8	*vLimit = (u8 *)(V_MEM_BASE + THE_LAST_LINE);

	pDst = (u8 *)(V_MEM_BASE + pTty->t_console.con_orig_addr);
	pSrc = (u8 *)(V_MEM_BASE + pTty->t_console.con_orig_addr + (ONE_LINE_SIZE << 1));

	while(pSrc < vLimit) {
		memcpy(vBuf, pSrc, ONE_LINE_SIZE);
		memcpy(pDst, vBuf, ONE_LINE_SIZE);

		pDst += ONE_LINE_SIZE;
		pSrc += ONE_LINE_SIZE;
	}
}


/*-----console_scroll_screen-----*/
void console_scroll_screen(int sToward, int sType, struct ttybuf *pTty)
{
	struct	video	*pCon = &pTty->t_console;

	if(sToward == P_UP) {
		if(pCon->con_cur_staddr > pCon->con_orig_addr) {
			if(sType == LINE) {
				pCon->con_cur_staddr -= ONE_LINE_SIZE;
			} else if(sType == PAGE) {
				pCon->con_cur_staddr = (pCon->con_cur_staddr > pCon->con_orig_addr + ONE_PAGE_SIZE) ? 
				pCon->con_cur_staddr - ONE_PAGE_SIZE : pCon->con_orig_addr;
			}
		}
	} else if(sToward == P_DOWN) {
		if(pCon->con_cur_staddr < pCon->con_orig_addr + THE_LAST_LINE) {
			if(sType == LINE) {
				pCon->con_cur_staddr += ONE_LINE_SIZE;
			} else if(sType == PAGE) {
				u16	cLimit = pCon->con_orig_addr + THE_LAST_PAGE;

				pCon->con_cur_staddr = (pCon->con_cur_staddr < cLimit) ? 
				pCon->con_cur_staddr + ONE_PAGE_SIZE : cLimit;
			}
		}
	}

	console_reflash(pTty);
}


/*-----console_panning_cursor-----*/
void console_panning_cursor(int pType, struct ttybuf *pTty)
{
	if(pType == C_RIGHT) {
		pTty->t_console.con_cursor -= 2;
	} else if(pType == C_LEFT){
		pTty->t_console.con_cursor += 2;
	}

	console_reflash(pTty);
}


/*-----console_shift_screen-----*/
void console_shift_screen(int nr_console)
{
	struct	ttybuf	*pTty = &ttyTab[nr_console];

	nr_current_console = nr_console;
	console_reflash(pTty);
}


/*-----console_clean_screen-----*/
void console_clean_screen(struct ttybuf *pTty)
{
	u8	*pZero, zeroBuf[ONE_LINE_SIZE] = {0};
	u8	*pVmem = (u8 *)(V_MEM_BASE + pTty->t_console.con_orig_addr);
	u8	*p_limit = pVmem + THE_LAST_LINE;

	for(pZero = zeroBuf; pZero < zeroBuf + ONE_LINE_SIZE; ) {
		*pZero++ = ' ';
		*pZero++ = LIGHT;
	}

	while(pVmem < p_limit) {
		memcpy(pVmem, zeroBuf, ONE_LINE_SIZE);
		pVmem += ONE_LINE_SIZE;
	}

	pTty->t_console.con_cursor = pTty->t_console.con_cur_staddr = pTty->t_console.con_orig_addr;
	console_set_head(pTty - ttyTab);
	console_reflash(pTty);
}


/*-----is_current_console-----*/
inline int is_current_console(int nConsole)
{
	return	(nConsole - nr_current_console);
}


/*-----console_reflash-----*/
static void console_reflash(struct ttybuf *pTty)
{
	if(!is_current_console(pTty - ttyTab)) {
		console_set_staddr(pTty->t_console.con_cur_staddr);
		console_set_cursor(pTty->t_console.con_cursor);
	}
}


/*-----console_set_cursor-----*/
static void console_set_cursor(u32 cPos)
{
	close_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H, 0);
	out_byte(CRTC_DATA_REG, ((cPos >>= 1) >> 8) & 0xFF, 0);
	out_byte(CRTC_ADDR_REG, CURSOR_L, 0);
	out_byte(CRTC_DATA_REG, cPos & 0xFF, 0);
	open_int();
}


/*-----set_video_start_addr-----*/
static void console_set_staddr(u32 sAddr)
{
	close_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H, 0);
	out_byte(CRTC_DATA_REG, ((sAddr >>= 1) >> 8) & 0xFF, 0);
	out_byte(CRTC_ADDR_REG, START_ADDR_L, 0);
	out_byte(CRTC_DATA_REG, sAddr & 0xFF, 0);
	open_int();
}


/*-----console_set_head-----*/
static void console_set_head(int nConsole)
{
	struct	ttybuf	*pTty = &ttyTab[nConsole];
	char		*pStr, headStr[LCONSOLE_HEAD] = "?#: ";

	headStr[0] = nConsole + 0x30;
	
	for(pStr = headStr; pStr < headStr + LCONSOLE_HEAD; pStr++)
		console_out_char(*pStr, pTty);
}
