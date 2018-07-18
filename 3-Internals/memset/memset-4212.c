/*
 * 'SUB-OPTIMAL MEMSET' RESEARCH
 * Reportedly, the memset of memory that is about to be free()d is 'optimized' out
 *	of the code when used on a heap-allocated buffer that is free()d immediately following
 *	the call to memset().
 * First, I want to see for myself (through experimentation)
 * Second, I want to find solutions that aren't optimized out
 * Third, I want to document my findings
 * Fourth, I want to implement a version of memset in Memoroad that is *not* 'optimized' out.
 *
 */
 
/*
 *	Thing -  4. mmap() memory
 *	Trick -  2. Volatile
 *	Object - 1. memset is called from a function
 *	Scheme - 2. memset is called from a local function
 */

#include <errno.h>			// errno
#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()
#include <sys/mman.h>		// mmap macros

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-4212


volatile char* volatile_harkleset(volatile char* oldBuff, char newChar, size_t buffLen);


int main(void)
{	
	volatile void *mapMem_ptr = NULL;
	size_t buffLen = strlen(WRAP_IT(SOURCE_NAME));
	int errNum = 0;
	errno = 0;
	
	// 0. Allocate it
	mapMem_ptr = mmap(NULL, buffLen + 1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	if (!mapMem_ptr)
	{
		HARKLE_ERROR(SOURCE_NAME, main, map_anon failed);
	}
	else
	{
		// 1. Use it
		if (mapMem_ptr != strncpy(mapMem_ptr, WRAP_IT(SOURCE_NAME), buffLen))
		{
			HARKLE_ERROR(SOURCE_NAME, main, strcpy failed);
		}
		else
		{
			puts(mapMem_ptr);

			// 2. volatile_harkleset() it
			if (mapMem_ptr != volatile_harkleset(mapMem_ptr, 'H', buffLen))
			{
				HARKLE_ERROR(SOURCE_NAME, main, volatile_harkleset failed);
			}
		}
	}
	
	// 3. Clean Up
	if (mapMem_ptr)
	{
		if (munmap(mapMem_ptr, buffLen + 1))
		{
			errNum = errno;
			HARKLE_ERROR(SOURCE_NAME, main, volatile_harkleset failed);
			HARKLE_ERRNO(SOURCE_NAME, munmap, errNum);
		}
	}
	
	// 4. Done
	return 0;
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
