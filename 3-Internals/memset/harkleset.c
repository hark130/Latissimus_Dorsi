#include <string.h>								// memset()


volatile char* volatile_harkleset_custom(volatile char* oldBuff, char newChar, size_t buffLen)
{
	// LOCAL VARIABLES
	volatile char* retVal = NULL;
	int i = 0;

	// INPUT VALIDATION
	if (oldBuff && buffLen > 0)
	{
		// REPLICATE MEMSET
		retVal = oldBuff;

		for (i = 0; i < buffLen; i++)
		{
			(*(retVal + i)) = newChar;
		}
	}

	// DONE
	return retVal;
}


volatile char* volatile_harkleset(volatile char* oldBuff, char newChar, size_t buffLen)
{
	// LOCAL VARIABLES
	volatile char* retVal = NULL;
	int i = 0;

	// INPUT VALIDATION
	if (oldBuff && buffLen > 0)
	{
		// MEMSET
		retVal = memset(oldBuff, newChar, buffLen);
	}

	// DONE
	return retVal;
}


void *harkleset(void *s, int c, size_t n)
{
	void *(*func_ptr)(void*, int, size_t) = memset;
	return func_ptr(s, c, n);
}
