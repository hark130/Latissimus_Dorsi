#include <string.h>								// memset()


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
