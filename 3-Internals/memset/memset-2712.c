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
 *	Trick -  7. explicit_bzero trick
 *	Object - 1. memset is called from a function
 *	Scheme - 2. memset is called from a local function
 */

#include "Harklerror.h"		// HARKLE_ERROR
#include <stdio.h>			// puts()
#include <string.h>			// memset()

#define WRAP_IT(thing) NO_REALLY_I_MEAN_IT(thing)
#define NO_REALLY_I_MEAN_IT(thing) #thing
#define SOURCE_NAME memset-2712

char buff[] = { WRAP_IT(SOURCE_NAME) };
	

void *harklexplicit(void *s, int c, size_t n);


int main(void)
{
	// 0. Local Variables
	size_t buffLen = sizeof(buff);
		
	// 1. Use it
	puts(buff);
	
	// 2. memset() it
	if (buff != harklexplicit(buff, 'H', buffLen))
	{
		HARKLE_ERROR(SOURCE_NAME, main, harklexplicit failed);
	}
	
	// 3. Done
	return 0;
}


void *harklexplicit(void *s, int c, size_t n)
{
	void *(*func_ptr)(void*, int, size_t) = memset;
	return func_ptr(s, c, n);
}
