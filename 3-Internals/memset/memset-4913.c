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
 *	Trick -  9. Testing harkleset from Memoroad.h
 *	Object - 1. memset is called from a function
 *	Scheme - 3. memset is called from a Header compiled/linked in
 */

#include <errno.h>			// errno
#include "Harklerror.h"		// HARKLE_ERROR
#include "Memoroad.h"		// harkleset()
#include <stdio.h>			// puts()
#include <sys/mman.h>		// mmap macros

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-4913


int main(void)
{	
	void *mapMem_ptr = NULL;
	size_t buffLen = strlen(WRAP_IT(SOURCE_NAME));
	int errNum = 0;
	errno = 0;
	
	// 0. Allocate it
	mapMem_ptr = mmap(NULL, buffLen + 1, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	
	if (!mapMem_ptr)
	{
		errNum = errno;
		HARKLE_ERROR(SOURCE_NAME, main, mmap failed);
		HARKLE_ERRNO(SOURCE_NAME, mmap, errNum);
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

			// 2. harkleset() it
			if (mapMem_ptr != harkleset(mapMem_ptr, 'H', buffLen))
			{
				HARKLE_ERROR(SOURCE_NAME, main, harkleset failed);
			}
		}
	}
	
	// 3. Clean Up
	if (mapMem_ptr)
	{
		if (munmap(mapMem_ptr, buffLen + 1))
		{
			errNum = errno;
			HARKLE_ERROR(SOURCE_NAME, main, munmap failed);
			HARKLE_ERRNO(SOURCE_NAME, munmap, errNum);
		}
	}
	
	// 4. Done
	return 0;
}
