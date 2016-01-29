#ifndef	_MUSE_CLOCK_H_
#define	_MUSE_CLOCK_H_

#define	TIMER_FREQ	1193182L
#define	HZ		100

#define	RATE_GENERATOR	0x34
#define	TIMER0		0x40

#define	TIMER_MODE	0x43

/* function proto */
void	clock_handler(void);

#endif
