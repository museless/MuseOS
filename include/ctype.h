#ifndef	_MUSE_CTYPE_H
#define	_MUSE_CTYPE_H

/* char check macro */
#define	isupper(c)	(c >= 65 && c <= 90)
#define	islower(c)	(c >= 97 && c <= 122)
#define	isdight(c)	(c >= 48 && c <= 57)
#define	isalpha(c)	(isupper(c) || islower(c))
#define	isalnum(c)	(isalpha(c) || isdight(c))
#define	isblank(c)	(c == ' ' || c == '	')
#define	ispath(c)	(c == '/')

#endif
