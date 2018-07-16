#ifndef __HARKLESET_H__
#define __HARKLESET_H__


/*
 *	PURPOSE - Call memset from a volatile function
 *	INPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	OUTPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	NOTES
 *		Originally written for memset-1212.c
 */
volatile char* volatile_harkleset(volatile char* oldBuff, char newChar, size_t buffLen);


#endif  // __HARKLESET_H__
