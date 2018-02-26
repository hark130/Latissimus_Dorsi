#ifndef __MEMOROAD__
#define __MEMOROAD__

#include <stdbool.h>	// bool, true, false
#include <stddef.h>		// size_t

//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
    Purpose - Abstract-away copying char arrays into heap-allocated arrays
    Input
        char_ptr - Nul-terminated character array
    Output - A heap-allocated char array containing a copy of char_ptr
    Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
 */
char* copy_a_string(const char* char_ptr);


//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a heap-allocated char array
	Output - On success, true. Otherwise, false.
	Notes:
		Will attempt to memset, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray);
 */
bool release_a_string(char** charPtr_ptr);


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a heap-allocated char array
		buffSize - The size, in memory, of *charPtr_ptr
	Output - On success, true. Otherwise, false.
	Notes:
		Will memset buffSize byptes, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray, buffSize);
 */
bool release_a_string_len(char** charPtr_ptr, size_t buffSize);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __MEMOROAD__