#ifndef	_MUSE_TYPE_H_
#define	_MUSE_TYPE_H_

#define	PUBLIC	
#define	PRIVATE	static	

#define	NR_IMBUF	20

typedef	unsigned int	u32;
typedef	unsigned short	u16;
typedef unsigned char	u8;

typedef	char*		va_list;

typedef	void		(*int_handler)(void);
typedef	void		(*task_f)(void);
typedef	void		*sys_call;


struct	data1 {
	int	a1;
	int	a2;
	int	a3;
	int	a4;
	void*	a5;
	void*	a6;
};

typedef	struct	message {
	u32	type;
	u32	source;

	union {
		struct	data1	d1;
	} m;
}MESSAGE;

typedef	struct	tid_inform_wait_queue {
	int	*buf_head;
	int	*buf_tail;

	int	buf[NR_IMBUF];
	int	buf_count;
}QUEUE;

#endif
