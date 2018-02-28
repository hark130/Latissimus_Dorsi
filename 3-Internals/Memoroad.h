#ifndef __MEMOROAD__
#define __MEMOROAD__

#include <stdbool.h>	// bool, true, false
#include <stddef.h>		// size_t

//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Allocate a buffer of size length + 1
	Input
		length - The length of what you want to store
	Ouput - Heap-allocated, memset, buffer of size length + 1
	Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
 */
char* get_me_a_buffer(size_t length);



/*
	Purpose - Allocate an array of char* on the heap
	Input
		arraySize - Number of char*s to include in the array
		nullTerm - true if this array is to be NULL terminated, 
			false otherwise
	Ouput - Heap-allocated, memset, char* array
	Notes:
        To explain nullTerm another way, if nullTerm is true,
			the actual number of indices allocated will be
			arraySize + 1
 */
char** get_me_a_buffer_array(size_t arraySize, bool nullTerm);


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
		Will memset buffSize bytes, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray, buffSize);
 */
bool release_a_string_len(char** charPtr_ptr, size_t buffSize);


/*
	Purpose - Ease the suffering of free()ing a char** from parse_proc_PIDs()
	Input
		charArr_ptr - A pointer to an array of char pointers
	Output - true on success, false on failure
	Notes:
		All C-strings will be memset to 0, free()d, and made NULL
		The array of C-strings will then be free()d and made NULL
 */
bool free_char_arr(char*** charArr_ptr);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __MEMOROAD__
