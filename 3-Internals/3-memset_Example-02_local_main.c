
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


int main(void)
{
	char buff[] = { "3-memset_Example-02_local_main.c" };
	size_t buffLen = sizeof(buff) / sizeof(*buff);
	int i = 0;
	
	// 1. Use it
	puts(buff);
	
	// 2. memset() it
	if (buff != memset(buff, '2', buffLen))
	{
		HARKLE_ERROR(memset Example 01, main, memset failed);
	}
	
	return 0;
}
