
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
 *	Thing -  2. Global scope
 *	Trick -  1. No tricks
 *	Object - 1. memset is called from a function
 *	Scheme - 1. memset is called from main()
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()

char buff[] = { "memset-2111.c" };

int main(void)
{	
	size_t buffLen = sizeof(buff);
	int i = 0;
	
	// 1. Use it
	puts(buff);
	
	// 2. memset() it
	if (buff != memset(buff, 'H', buffLen))
	{
		HARKLE_ERROR(memset 2111, main, memset failed);
	}
	
	// 3. Done
	return 0;
}
