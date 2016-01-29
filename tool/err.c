#include "apue.h"
#include <errno.h>
#include <stdarg.h>

static	void 	err_doit(int, int, const char*, va_list);


/*-----------err_ret------------*/
void err_ret(const char* fmt,...)
{
	va_list	ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}


/*----------err_exit------------*/
void err_exit(int error,const char* fmt,...)
{
	va_list	ap;

	va_start(ap,fmt);
	err_doit(1,error,fmt,ap);
	va_end(ap);
	exit(1);
}


/*------------err_quit----------*/
void  err_quit(const char* fmt,...)
{
	va_list	ap;
	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);
	exit(1);
}


/*------------err_sys-----------*/
void err_sys(const char* fmt,...)
{
	va_list	ap;
	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	exit(1);
}


/*----------err_doit------------*/
static void err_doit(int errnoflag, int error, const char* fmt, va_list ap)
{
	char	buf[MAXLINE];

	vsnprintf(buf, MAXLINE, fmt, ap);
	if(errnoflag) 
		snprintf(buf + strlen(buf), MAXLINE - strlen(buf), ": %s", strerror(error));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}

