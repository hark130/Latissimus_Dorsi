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
    // LOCAL VARIABLES
    char** retVal = NULL;
    dirDetails_ptr procDetails_ptr = NULL;

    // 1. walk_proc()
    procDetails_ptr = walk_proc();

    // 2. parse_PID_dirs_to_arr()
    if (procDetails_ptr)
    {
        retVal = parse_PID_dirs_to_arr(procDetails_ptr);
    }

    // 3. free_dirDetails_ptr()
    if (procDetails_ptr)
    {
        free_dirDetails_ptr(&procDetails_ptr);
    }

    // DONE
    return retVal;
}


/*
	Purpose - Walk the /proc directory for a list of files and dirs
	Input - None
	Output - A pointer to a dirDetails struct as defined in Harkledir.h
	Notes:
		It is your responsibility to call free_dirDetails_ptr(&dirDetails_ptr)
 */
dirDetails_ptr walk_proc(void)
{
    // LOCAL VARIABLES
    dirDetails_ptr retVal = open_dir("/proc");

    if (!dirDetails_ptr)
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - walk_proc() - open_dir failed!\n");
    }

    // DONE
    return retVal;
}


/*
	Purpose - Abstract-away the translation of a dirDetail array of directories into an array of char pointers
	Input
		procWalk_ptr - A dirDetails_ptr of /proc
	Output - A NULL-terminated, heap-allocated array of heap-allocated C-strings, one-per-directory
	Notes:
		This function will ignore files and filter out non-<PID> directories in /proc
 */
char** parse_PID_dirs_to_arr(dirDetails_ptr procWalk_ptr)
{
    // LOCAL VARIABLES
    char** retVal = NULL;  // Return value
    char** temp_ptr = NULL;  // Iterating variable
    char** tempFN_ptr = NULL;  // Iterating variable for the dirName_arr
    int numTries = 0;  // Tracks attempts to avoid upper end limit of allocation attempts
    int numNames = 0;  // Number of names to add
    int i = 0;  // Loop iterating variable

    // INPUT VALIDATION
    if (procWalk_ptr)
    {
        // Allocate the array of char pointers
        if (procWalk_ptr->numDirs > 0 && procWalk_ptr->dirName_arr)
        {
            numNames = procWalk_ptr->numDir;
            tempFN_ptr = procWalk_ptr->dirName_arr;
            while (retVal == NULL && numTries < HPROC_MAX_TRIES)
            {
                retVal = (char**)calloc(numNames, sizeof(char*));
                numTries++;
            }

            if (retVal)
            {
                temp_ptr = retVal;
                
                for (i = 0; i < numNames; i++)
                {
                    (*temp_ptr) = copy_a_name(*tempFN_ptr);

                    if(!(*temp_ptr))
                    {
                        fprintf(stderr, "<<<ERROR>>> - Harkleproc - parse_PID_dirs_to_arr() - copy_a_name failed!\n");
                    }

                    temp_ptr++;
                    tempFN_ptr++;
                }
            }
            else
            {
                fprintf(stderr, "<<<ERROR>>> - Harkleproc - parse_PID_dirs_to_arr() - calloc failed!\n");
            }
        }
        else
        {
            fprintf(stderr, "<<<ERROR>>> - Harkleproc - parse_PID_dirs_to_arr() - invalid dirDetails struct!\n");
        }
    }
    else
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - parse_PID_dirs_to_arr() - NULL pointer!\n");
    }

    // DONE
    return retVal;
}


bool free_char_arr(char*** charArr_ptr)
{
    // LOCAL VARIABLES
    bool retVal = true;
    char* currChar_ptr = NULL;  // C-string to be cleared
    char* temp_ptr = NULL;  // Return value from memset
    char** currChar_arr = NULL;  // Array of C-strings to be cleared
    size_t currLen = 0;  // Length of the current string

    // INPUT VALIDATION
    if (charArr_ptr)
    {
        if (*charArr_ptr)
        {
            currChar_arr = *charArr_ptr;

            while (*currChar_arr)
            {
                currChar_ptr = *currChar_arr;

                if (currChar_ptr)
                {
                    // memset char*
                    if (*currChar_ptr)
                    {
                        currLen = strlen(currChar_ptr);

                        if (currLen > 0)
                        {
                            temp_ptr = memset(currChar_ptr, 0x0, currLen);

                            if (temp_ptr != currChar_ptr)
                            {
                                fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_char_arr() - memset failed!\n");
                                retVal = false;
                            }
                            else
                            {
                                temp_ptr = NULL;
                            }
                        }
                    }

                    // free char*
                    free(currChar_ptr);

                    // NULL char*
                    currChar_ptr = NULL;
                    *currChar_arr = NULL;
                }
                // Next char*
                currChar_ptr++;
            }

            // free char**
            free(*charArr_ptr);

            // NULL char**
            charArr_ptr = NULL;
        }
        else
        {
            fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_char_arr() - NULL pointer!\n");
            retVal = false;
        }
    }
    else
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_char_arr() - NULL pointer!\n");
        retVal = false;
    }

    // DONE
    return retVal;
}


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

