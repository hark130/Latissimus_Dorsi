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
 *	Trick -  1. No tricks
 *	Object - 1. memset is called from a function
 *	Scheme - 1. memset is called from main()
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()


int main(void)
{	
	char* buff = NULL;
	size_t buffLen = strlen("memset-3111.c");
	int i = 0;
	
	// 0. Allocate it
	buff = (char*)calloc(buffLen + 1, sizeof(char));
	
	if (!buff)
	{
		HARKLE_ERROR(memset-3111, main, calloc failed);
	}
	else
	{
		// 1. Use it
		puts(buff);

		// 2. memset() it
		if (buff != memset(buff, 'H', buffLen))
		{
			HARKLE_ERROR(memset-3111, main, memset failed);
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
