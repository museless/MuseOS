/* Now I only support one hd */
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proc.h"
#include "hd.h"
#include "command.h"
#include "proto.h"


/* local function */
static	void	hd_identify(void);
static	void	hd_cmd_out(struct hd_com_reg *hComReg);
static	void	hd_get_partition_info(int nDrive);
static	int	hd_get_part_table(int nDrive, struct partition *hd, int startLba);
static	void	hd_rdwt(int pSrc, int nDrive, int nLba, int nSize, void *rdBuf, int wrFlags);
static	void	hd_ioctl(MESSAGE *tranMsg);
static	void	hd_geterr(int nDrive, u8 *errno);
static	void	hdqueue_init(void);

static	void	hd_print_identify(u16 *hdBuf);
static	void	hd_print_partition(void);

/* local inline function */
static	inline	int	hd_wait(int runTime, int maskVal, int waitVal);
static	inline	void	hd_set_switch(int switchFlags);
static	inline	void	hd_inform(int procOff, u32 reqCom);
static	inline	void	hd_server(int ctlVal, struct message *ctlMsg);

/* static data */
static	struct	partition	primPart[NR_PART_PER_HD];
static	struct	partition	logPart[NR_LOG_PER_HD];
static	struct	part_lba	primLbaInfo[NR_PART_PER_HD];
static	struct	part_lba	logLbaInfo[NR_LOG_PER_HD];
static	QUEUE			hdQueue;		
static	u8			hdBuf[MAXBUFSIZE];
static	u8			initFlags;


/*-----hd_init-----*/
void hd_init(void)
{
	prot_set_ihandler(hd_handler, AT_WINI_IRQ);
	enable_intr(CASCADE_IRQ);
	enable_intr(AT_WINI_IRQ);
}


/*-----hdqueue_init-----*/
void hdqueue_init(void)
{
	hdQueue.buf_tail = hdQueue.buf_head = hdQueue.buf;
	hdQueue.buf_count = NR_IMBUF;
	
	hd_set_switch(HD_OPEN);
}


/*-----hd_platform-----*/
void hd_platform(void)
{	
	struct	message		hdMsg;

	hd_identify();	
	hd_get_partition_info(0);
	hd_print_partition();

	while(1) {
		send_recv(RECEIVE, SOLDIER, &hdMsg);
		hd_server(hdMsg.type, &hdMsg);
	}
}


/*-----hd_server-----*/
/* This function is a connector */
/* to all operation in hd */
static inline void hd_server(int ctlVal, struct message *ctlMsg)
{
	PLBA	*pLpart;
	char	rwFlags;
	int	nLba = ctlMsg->LBA;

	if(ctlVal == IOCTL) {
		hd_ioctl(ctlMsg);
	} else if((rwFlags = (ctlVal == WRITE) ? 1 : ((ctlVal == READ) ? 0 : -1)) != -1) {
		pLpart = (ctlMsg->DEVICE <= NR_PART_PER_HD) ? primLbaInfo : logLbaInfo;
		nLba += pLpart[ctlMsg->DEVICE].pl_base;
		hd_rdwt(ctlMsg->source, ctlMsg->DRIVE, nLba, ctlMsg->SIZE, ctlMsg->BUF, rwFlags);	
	} else {
		printx(MAGIC_KERN, "Unknown ioctl command %d\n", ctlVal);
	}
	
	unblock(ctlMsg->source);
}


/*-----hd_handler-----*/
void hd_handler(void)
{
	in_byte(P_STATUS);	/* clean the status buf to keep run */
	hd_inform(SCOUT, ESTABLISH_PORT);
}


/*-----hd_identify-----*/
static void hd_identify(void)
{
	struct	hd_com_reg	hdReg;

	hdReg.hc_command = IDENTIFY_COM;
	hdReg.hc_device = MAKE_DEV_REG(0, 0, 0);

	hd_cmd_out(&hdReg);
	dev_ready(SCOUT);

	port_read(P_DATA, hdBuf, IDEN_DEV_DAT_L);
	hd_print_identify((u16 *)hdBuf);

	return;
}


/*-----hd_get_partition_info-----*/
static void hd_get_partition_info(int nDrive)
{
	int	nCir;

	hd_rdwt(DF_HD, nDrive, 0, BYTE_PER_SECT, hdBuf, 0);
	memcpy(primPart, hdBuf + PART_TAB_OFF, PART_TABLE_LEN);

	for(nCir = 0; nCir < NR_PART_PER_HD; nCir++) {
		if(primPart[nCir].pt_system_id == 0)
			continue;

		primLbaInfo[nCir] = primPart[nCir].pt_lba_info;

		if(primPart[nCir].pt_system_id == EXTENDED) {
			int	logCir, fRet = 1;
			int	logOff = nCir * NR_LOG_PER_PART;
			int	startLba = primPart[nCir].pt_lba_info.pl_base;

			for(logCir = 0; logCir < NR_LOG_PER_PART && fRet; logCir++) {
				fRet = hd_get_part_table(nDrive, &logPart[logOff + logCir], startLba);
				logLbaInfo[logOff + logCir] = logPart[logOff + logCir].pt_lba_info;
				startLba = logLbaInfo[logOff + logCir].pl_base + logLbaInfo[logOff + logCir].pl_size;
			}
		}
	}

	return;
}


/*-----hd_rdwt-----*/
static void hd_rdwt(int pSrc, int nDrive, int nLba, int nSize, void *rdBuf, int wrFlags)
{
	struct	hd_com_reg	hComReg;

	hComReg.hc_features = 0;
	hComReg.hc_sector_count = nSize / BYTE_PER_SECT + (nSize % BYTE_PER_SECT ? 1 : 0);
	hComReg.hc_lba_low = nLba & 0xFF;
	hComReg.hc_lba_mid = (nLba >> 8) & 0xFF;
	hComReg.hc_lba_high = (nLba >> 16) & 0xFF;
	hComReg.hc_device = MAKE_DEV_REG(1, nDrive, nLba);
	hComReg.hc_command = (wrFlags ? WRITE_COM : READ_COM);

	hd_cmd_out(&hComReg);

	char	*pBuf = prot_vatoma(pSrc, rdBuf);
	int	reqBytes = nSize;
	int	rwBytes;
	
	while(reqBytes) {
		rwBytes = (reqBytes >= BYTE_PER_SECT) ? BYTE_PER_SECT : reqBytes;

		if(wrFlags) {
			while(!hd_wait(HD_TIMEOUT, TRAN_SET, TRAN_SET))
				printx(MAGIC_KERN, "hd writing error");

			port_write(P_DATA, pBuf, rwBytes);
			dev_ready(SCOUT);
		} else {
			dev_ready(SCOUT);
			port_read(P_DATA, hdBuf, rwBytes);
			memcpy(pBuf, hdBuf, rwBytes);
		}

		pBuf += rwBytes;
		reqBytes -= rwBytes;
	}
}


/*-----hd_set_switch-----*/
static inline void hd_set_switch(int switchFlags)
{
	initFlags = (switchFlags == HD_OPEN) ? 1 : 0;
}


/*-----hd_ioctl-----*/
static void hd_ioctl(MESSAGE *tranMsg)
{
	PLBA	*pSrc, *rBuf;
	int	nDevice = tranMsg->DEVICE - ROOT_1a;

	if(nDevice > 0 && nDevice < NR_LOG_PER_HD) {
		rBuf = prot_vatoma(tranMsg->source, tranMsg->BUF);
		pSrc = (nDevice < NR_PART_PER_HD) ? primLbaInfo : logLbaInfo;
		memcpy(rBuf, (pSrc + nDevice), sizeof(PLBA));
	}
}


/*-----hd_geterr-----*/
static void hd_geterr(int nDrive, u8 *errno)
{

}


/*-----hd_get_part_table-----*/
static int hd_get_part_table(int nDrive, struct partition *hd, int startLba)
{
	hd_rdwt(DF_HD, nDrive, startLba, BYTE_PER_SECT, hdBuf, 0);
	memcpy((void *)hd, hdBuf + PART_TAB_OFF, ONE_PART_TAB_L);

	hd->pt_lba_info.pl_base = startLba + 0x3F;
	
	/* check the second partition table is the end or not */
	return	(((*(u8 *)(hdBuf + PART_TAB_OFF + ONE_PART_TAB_L + 4)) == 0) ? 0 : 1);
}


/*-----hd_cmd_out-----*/
static void hd_cmd_out(struct hd_com_reg *hComReg)
{
	if(!hd_wait(HD_TIMEOUT, BSY_SET, 0))
		printx(MAGIC_KERN, "hd error");

	out_byte(P_DEV_CONTROL, 0, 0);

	out_byte(P_FEATURES, hComReg->hc_features, 0);
	out_byte(P_SECTOR_COUNT, hComReg->hc_sector_count, 0);
	out_byte(P_LBA_LOW, hComReg->hc_lba_low, 0);
	out_byte(P_LBA_MID, hComReg->hc_lba_mid, 0);
	out_byte(P_LBA_HIGH, hComReg->hc_lba_high, 0);
	out_byte(P_DEVICE, hComReg->hc_device, 0);
	out_byte(P_COMMAND, hComReg->hc_command, 1);
}


/*-----hd_wait-----*/
static inline int hd_wait(int runTime, int maskVal, int waitVal)
{
	int	nCir;

	for(nCir = 0; nCir < runTime; nCir++) {
		if((in_byte(P_STATUS) & maskVal) == waitVal)
			return	1;
	}

	return	0;
}


/*-----hd_inform-----*/
static inline void hd_inform(int procOff, u32 reqCom)
{
	(proc_on_head + procOff)->proc_state = reqCom;
	unblock(procOff);
}


/*-----hd_access_ctl-----*/
void hd_access_ctl(u32 reqCom, struct message *tranMsg)
{
	PROC	*pDest = (proc_on_head + SOLDIER);

	if(!initFlags)
		hdqueue_init();

	access_mutex_lock(&hdQueue);

	pDest->proc_state = reqCom;
	pDest->proc_msg = ((tranMsg) ? tranMsg : NULL);

	block(tranMsg->source, SOLDIER, BLK_ONE_UN_TWO);

	access_mutex_unlock(&hdQueue);
}


/*-----dev_ready-----*/
void dev_ready(int procOff)
{
	struct	message	tranMsg;

	send_recv(RECEIVE, procOff, &tranMsg);

	if(tranMsg.type != LISTEN)
		printx(MAGIC_KERN, "dev_ready: ant send error msg, msg_source: %d", tranMsg.source);
}


/*-----hd_wrdata-----*/
void hd_wrdata(int nDrive, int nLba, int bSize, char *rBuf, int cType)
{
	MESSAGE	tranMsg;
	
	if(cType != READ && cType != WRITE)
		return;
	
	tranMsg.type = cType;
	tranMsg.source = proc_getpid();
	tranMsg.DEVICE = MINOR(nDrive) - ROOT_1a;
	tranMsg.DRIVE = NDRIVE(tranMsg.DEVICE);
	tranMsg.LBA = nLba;
	tranMsg.SIZE = bSize;
	tranMsg.BUF = rBuf;

	hd_access_ctl((cType == READ) ? GET_DATA : TRAN_DATA, &tranMsg);
}


/*-----hd_print_identify-----*/
static void hd_print_identify(u16 *hdBuf)
{
	int	pl_size = ((int)hdBuf[61] << 16) + hdBuf[60];

	printx(MAGIC_USR, "\nHD SIZE: %dMB\n", pl_size * 512 / 1048576);
}


/*-----hd_print_partition-----*/
static void hd_print_partition(void)
{
	int	nCir;

	for(nCir = 0; nCir < NR_LOG_PER_HD; nCir++)
		if(logLbaInfo[nCir].pl_base != 0)
			printx(MAGIC_USR, "P%d Base: %x S: %x\n", nCir, logLbaInfo[nCir].pl_base, logLbaInfo[nCir].pl_size);
}
