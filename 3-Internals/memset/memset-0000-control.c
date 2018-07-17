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
 * This file represents my control.
 *	1. I use a buffer
 *	2. I memset the buffer
 *	3. I use the buffer again
 * If compilers are 'optimizing' out calls to memset because the buffer to be memset
 *	is never used again, this code should avoid that by reusing the buffer in question.
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-0000


int main(void)
{
	char buff[] = { WRAP_IT(SOURCE_NAME) };
	size_t buffLen = sizeof(buff);
	int i = 0;
	
	// 1. Use it
	puts(buff);
	
	// 2. memset() it
	if (buff != memset(buff, 'H', buffLen - 1))
	{
		HARKLE_ERROR(SOURCE_NAME, main, memset failed);
	}
	else
	{
		// 3. Print it
		puts(buff);

		// 4. Use it again
		for (i = 0; i < buffLen - 1; i++)
		{
			buff[i] = 'a';
		}
		
		puts(buff);
	}
	
	return 0;
}
