#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false
#include <stdlib.h>     // calloc
#include <string.h>     // strlen

#ifndef HPROC_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HPROC_MAX_TRIES 3
#endif  // HPROC_MAX_TRIES


/*
    Purpose - Abstract-away copying names into heap-allocated arrays
    Input
        fileName - Nul-terminated character array
    Output - A heap-allocated char array containing a copy of fileName
    Notes:
        It is your responsibility to free the char* returned by this function
 */
char* copy_a_name(const char* fileName);


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
char** parse_proc_PIDs(void)
{
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
}


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


char* copy_a_name(const char* fileName)
{
    // LOCAL VARIABLES
    int numTries = 0;  // Max number to calloc attempts
    char* retVal = NULL;  // Allocated char array
    char* temp_ptr = NULL;  // Holds return value from memcpy
    size_t fNameLen = 0;  // Length of fileName
    
    // INPUT VALIDATION
    if (fileName)
    {
        if (*fileName)
        {    
            // 1. Size the fileName
            fNameLen = strlen(fileName);
            
            if (fNameLen > 0)
            {            
                // 2. Allocate a properly-sized array
                while (retVal == NULL && numTries < HPROC_MAX_TRIES)
                {
                    retVal = (char*)calloc(fNameLen + 1, sizeof(char));
                    numTries++;
                }
                
                if (retVal)
                {                
                    // 3. Copy fileName into that array
                    temp_ptr = (char*)memcpy((void*)retVal, (void*)fileName, fNameLen * sizeof(char));
                    
                    if (temp_ptr != retVal)
                    {
                        fprintf(stderr, "<<<ERROR>>> - Harkleproc - copy_a_name() - memcpy failed!\n");
                        // Free retVal
                        free(retVal);
                        // NULL retVal
                        retVal = NULL;
                    }
                }
                else
                {
                    fprintf(stderr, "<<<ERROR>>> - Harkleproc - copy_a_name() - calloc failed!\n");
                }                
            }
        }
    }
    
    // DONE
    return retVal;
}



