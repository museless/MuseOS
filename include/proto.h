/* this header include the function */
/* which can used by all the others */

/* lib_string_app.asm */
void*	memcpy(void* mDst, void* dSrc, u32 mLen);
void	memset(void* mDst, char mCh, u32 len);
char*	strncpy(char* mDst, char* dSrc, u32 mLen);
int	strlen(char* cStr);
int	strncmp(char *fCmp, char *sCmp, u32 mLen);

/* lib_port.asm */
void	out_byte(u16 port, u8 data, int type);
u8	in_byte(u16 port);

void	port_read(u16 port, void *buf, int len);
void	port_write(u16 port, void *buf, int len);

int	disable_intr(u16 port);
int	enable_intr(u16 port);

void	open_int(void);
void	close_int(void);

void	set_cr0_wp(void);

/* lib_disp.asm */
void	disp_str(char* str);
void	disp_color_str(char* str, int color);
void	disp_int(int figure);

/* lib_ebug.asm */
void	stop(void);
void	ele_stop(int data1, int data2);
u32	get_cr2();
u32	get_cr3();

/* Muse_syscall.asm */
int	syscall_sendrecv(int function, int dest, struct message *mes);
int	syscall_print(char magic, char *buf, int len);
int	syscall_block(int procOne, int procTwo, int blockType);

/* dev_keyboard_ctl.c */
void	keyboard_init(void);

/* dev_clock_ctl.c */
void	clock_init(void);

/* file_memory_ctl.c */
void	memory_init(void);

/* dev_hd_ctl.c */
void	hd_init(void);
void	hd_access_ctl(u32 reqCom, struct message *tranMsg);

/* musefs_main.c */
void	musefs_platform(void);

/* file_tty_ctl.c */
void	tty_init(void);

/* file_buffer_ctl.c */
void	buffer_init(void);

/* syscall_printx.c */
int	printx(char magic, char *fmt, ...);
int	sys_printx(char magic, char *buf, int len, int unused);

/* lib_string_ctl.c */
int	vsprintf(char *buf, char *fmt, va_list arg);
void 	itoa(int figure, char *str, int base);

/* lib_misc.c */
void	assertion_failure(char *eName, char *fName, char *baseFile, int nLine);
int	send_recv(int funType, int sDest, struct message *tranMsg);
int	block(int procOne, int procTwo, int blockType);

/* lib_access_lock.c */
int	access_mutex_lock(QUEUE *queue);
int	access_mutex_unlock(QUEUE *queue);

/* test.c */
void	test(void);


