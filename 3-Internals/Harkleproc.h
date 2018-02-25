#ifndef __HARKLEPROC__
#define __HARKLEPROC__

#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false


/*
	Purpose - Provide a 'list' of running PIDs based on /proc
	Input - None
	Ouput - A NULL-terminated, heap-allocated array of heap-allocated C-strings, one-per-directory
	Notes:
		Does all the 'heavy lifting' to walk /proc and filter out <PID> dirs
		This function calls walk_proc() to procure a list of /proc directories
		This function free()s the dirDetails_ptr resulting from walk_proc() before returning
		It is your responsibility to free each char* and the char** itself (or call free_char_arr())
 */
char** parse_proc_PIDs(void);


/*
	Purpose - Walk the /proc directory for a list of files and dirs
	Input - None
	Output - A pointer to a dirDetails struct as defined in Harkledir.h
	Notes:
		It is your responsibility to call free_dirDetails_ptr(&dirDetails_ptr)
 */
dirDetails_ptr walk_proc(void);


/*
	Purpose - Abstract-away the translation of a dirDetail array of directories into an array of char pointers
	Input
		procWalk_ptr - A dirDetails_ptr of /proc
	Output - A NULL-terminated, heap-allocated array of heap-allocated C-strings, one-per-directory
	Notes:
		This function will ignore files and filter out non-<PID> directories in /proc
 */
char** parse_PID_dirs_to_arr(dirDetails_ptr procWalk_ptr);


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


/*
	Purpose - Check a /proc directory name for all numbers
	Input
		dirName - nul-terminated char array holding a /proc directory name
	Output - true if dirName a non-empty string of numbers, false otherwise
	Notes:
		This function does not validate whether dirName is a directory
		This function does not validate whether dirName is in /proc
		This function merely was intentended to be used in this manner
 */
bool is_it_a_PID(const char* dirName);



/*
parse_proc_PIDs()
1. walk_proc() -> open_dir("/proc")
2. parse_PID_dirs_to_arr() -> copy_a_name() walks dirNames_arr for <PID> dirs adding them to a char**]
3. free_dirDetails_ptr()
4. return the char**
NOTE:  
	Implement copy_a_name() inside Harkleproc.c
	User must call free_char_arr() or do it manually
 */

#endif  // __HARKLEPROC__
