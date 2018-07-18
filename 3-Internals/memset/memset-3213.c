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
 *	Thing -  3. Heap memory
 *	Trick -  2. Volatile
 *	Object - 1. memset is called from a function
 *	Scheme - 3. memset is called from a Header compiled/linked in
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include "harkleset.h"		// volatile_harkleset()
#include <stdio.h>			// puts()
#include <stdlib.h>			// calloc()
#include <string.h>			// memset()

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-3213


int main(void)
{	
	volatile char* buff = NULL;
	size_t buffLen = strlen(WRAP_IT(SOURCE_NAME));
	
	// 0. Allocate it
	buff = (char*)calloc(buffLen + 1, sizeof(char));
	
	if (!buff)
	{
		HARKLE_ERROR(SOURCE_NAME, main, calloc failed);
	}
	else
	{
		// 1. Use it
		if (buff != strncpy(buff, WRAP_IT(SOURCE_NAME), buffLen))
		{
			HARKLE_ERROR(SOURCE_NAME, main, strncpy failed);
		}
		else
		{
			puts(buff);

			// 2. volatile_harkleset() it
			if (buff != volatile_harkleset(buff, 'H', buffLen))
			{
				HARKLE_ERROR(SOURCE_NAME, main, volatile_harkleset failed);
			}
		}
	}
	
	// 3. Clean Up
	if (buff)
	{
		free(buff);
	}
	
	// 4. Done
	return 0;
}
