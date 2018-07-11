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
 *	Thing -  1. Local scope
 *	Trick -  8. Touching Memory
 *	Object - 1. memset is called from a function
 *	Scheme - 1. memset is called from main()
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-1811


int main(void)
{
	char buff[] = { WRAP_IT(SOURCE_NAME) };
	size_t buffLen = sizeof(buff);
	int i = 0;
	
	// 1. Use it
	puts(buff);
	
	// 2. memset() it
	if (buff != memset(buff, 'H', buffLen))
	{
		HARKLE_ERROR(SOURCE_NAME, main, memset failed);
	}
	else
	{
		*(volatile char*)buff = *(volatile char*)buff;
	}
	
	// 3. Done
	return 0;
}
