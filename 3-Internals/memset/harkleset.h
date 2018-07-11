#ifndef __HARKLESET_H__
#define __HARKLESET_H__

/*
 *	PURPOSE - Replicate memset functionality without calling memset
 *	INPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	OUTPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	NOTES
 *		Originally written for memset-1212a.c
 *		Calling this function will require manually "reversing" since memset will not appear
 *			in the resulting ELF Section header.
 */
volatile char* volatile_harkleset_custom(volatile char* oldBuff, char newChar, size_t buffLen);


/*
 *	PURPOSE - Call memset from a volatile function
 *	INPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	OUTPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	NOTES
 *		Originally written for memset-1212.c
 */
volatile char* volatile_harkleset(volatile char* oldBuff, char newChar, size_t buffLen);


/*
 *	PURPOSE - Use a function pointer to call memset
 *	INPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	OUTPUT - http://man7.org/linux/man-pages/man3/memset.3.html
 *	NOTES
 *		Originally written for memset-1712.c
 */
void *harkleset(void *s, int c, size_t n);


#endif  // __HARKLESET_H__
