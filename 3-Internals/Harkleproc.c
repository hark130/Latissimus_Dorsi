#include "Harkledir.h"
#include "Harkleproc.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>
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


pidDetails_ptr create_PID_struct(void)
{
    // LOCAL VARIABLES
    pidDetails_ptr retVal = NULL;
    int numTries = 0;

    while (!retVal && numTries < HPROC_MAX_TRIES)
    {
        retVal = (pidDetails_ptr)calloc(1, sizeof(pidDetails));
        numTries++;
    }

    if (!retVal)
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - create_PID_struct() - Failed to allocate a pidDetails struct pointer!\n");
    }

    // DONE
    return retVal;
}


/*
    Purpose - Populate a harklePIDDetails struct with PID information
    Input - None
    Output
        A pointer to a heap-allocated harklePIDDetails struct complete
            with information about the PID found at pidPath on success.
            NULL on failure.
    Notes:
        Returns NULL if pidPath does not begin with /proc
        Returns NULL if the directory following /proc is not a PID (non-number)
        Returns a pointer if pidPath is missing but stillExists is False
 */
pidDetails_ptr populate_PID_struct(const char* pidPath);


bool free_PID_struct(pidDetails_ptr* pidDetails_ptr)
{
    // LOCAL VARIABLES
    bool retVal = true;
    pidDetails_ptr tmpStruct_ptr = NULL;  // Easier to deal with this way
    char* tmp_ptr = NULL;  // Easier to deal with char array pointers this way
    char* mem_ptr = NULL;  // Return value from memset function calls
    size_t length = 0;  // Length of a char array

    // INPUT VALIDATION
    if (pidDetails_ptr)
    {
        if (*pidDetails_ptr)
        {
            tmpStruct_ptr = *pidDetails_ptr;

            // 1. char* pidName;          // Absolute path of PID
            if (tmpStruct_ptr->pidName)
            {
                tmp_ptr = tmpStruct_ptr->pidName;

                if (*tmp_ptr)
                {
                    // 1.1. memset pidName
                    length = strlen(tmp_ptr);

                    if (length > 0)
                    {
                        mem_ptr = memset(tmp_ptr, 0x0, length);

                        if (mem_ptr != temp_ptr)
                        {                            
                            fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - memset failed!\n");
                            retVal = false;
                        }
                    }

                    // 1.2. free pidName
                    free(tmp_ptr);

                    // 1.3. NULL pidName
                    tmp_ptr = NULL;
                    tmpStruct_ptr->pidName = NULL;
                }
            }

            // 2. char* pidCmdline;       // Complete cmdline used to execute the PID
            if (tmpStruct_ptr->pidCmdline)
            {
                tmp_ptr = tmpStruct_ptr->pidCmdline;

                if (*tmp_ptr)
                {
                    // 1.1. memset pidCmdline
                    length = strlen(tmp_ptr);

                    if (length > 0)
                    {
                        mem_ptr = memset(tmp_ptr, 0x0, length);

                        if (mem_ptr != temp_ptr)
                        {
                            fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - memset failed!\n");
                            retVal = false;
                        }
                    }

                    // 1.2. free pidCmdline
                    free(tmp_ptr);

                    // 1.3. NULL pidCmdline
                    tmp_ptr = NULL;
                    tmpStruct_ptr->pidCmdline = NULL;
            }

            // 3. bool stillExists;       // False if PID ever disappears
            // Just in case someone would think about accessing this
            tmpStruct_ptr->stillExists = false;
            
            // 4. NULL pidDetails_ptr
            *pidDetails_ptr = NULL;
        }
        else
        {
            fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - NULL pointer!\n");
            retVal = false;
        }
    }
    else
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - NULL pointer!\n");
        retVal = false;
    }

    // DONE
    return retVal;
}


/*
    Purpose - To memset, free, and NULL all harklePIDDetails pointers
        in an array, to include the array itself
    Input
        pidDetails_arr - Pointer to a NULL-terminated array of harklePIDDetails pointers
    Output - True on success, False on failure
    Notes:
        Calls free_PID_struct() to free each pidDetails_ptr in the array
        Will free the array itself
        Will NULL the array pointer
 */
bool free_PID_struct_arr(pidDetails_ptr** pidDetails_arr)
{
    // LOCAL VARIABLES
    bool retVal = true;
    bool freeReturn = true;
    pidDetails_ptr currStruct_ptr = NULL;  // Will hold each struct pointer in the array
    pidDetails_ptr* currStruct_arr = NULL;  // Will hold the pointer to the array

    // INPUT VALIDATION
    if (pidDetails_arr)
    {
        if (*pidDetails_arr)
        {
            currStruct_arr = *pidDetails_ptr;

            // 1. Free each struct pointer
            while (*currStruct_arr)
            {
                currStruct_ptr = *currStruct_arr;

                fprintf(stdout, "BEFORE currStruct_ptr:\t%p\n", currStruct_ptr);  // DEBUGGING
                fprintf(stdout, "BEFORE *currStruct_arr:\t%p\n", *currStruct_arr);  // DEBUGGING
                freeReturn = free_PID_struct(&currStruct_ptr);
                fprintf(stdout, "AFTER currStruct_ptr:\t%p\n", currStruct_ptr);  // DEBUGGING
                fprintf(stdout, "AFTER *currStruct_arr:\t%p\n", *currStruct_arr);  // DEBUGGING

                if (freeReturn == false)
                {
                    retVal = false;
                }

                // Next struct pointer
                currStruct_arr++;
            }

            // 2. Free the struct array
            free(*pidDetails_arr);

            // 3. NULL the struct array
            *pidDetails_arr = NULL;
        }
        else
        {
            fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - NULL pointer!\n");
            retVal = false;
        }
    }
    else
    {
        fprintf(stderr, "<<<ERROR>>> - Harkleproc - free_PID_struct() - NULL pointer!\n");
        retVal = false;
    }

    // DONE
    return retVal;
}


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
    // puts("1. walk_proc()");  // DEBUGGING
    procDetails_ptr = walk_proc();

    // 2. parse_PID_dirs_to_arr()
    // puts("2. parse_PID_dirs_to_arr()");  // DEBUGGING
    if (procDetails_ptr)
    {
        retVal = parse_PID_dirs_to_arr(procDetails_ptr);
    }

    // 3. free_dirDetails_ptr()
    // puts("3. free_dirDetails_ptr()");  // DEBUGGING
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

    if (!retVal)
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
            numNames = procWalk_ptr->numDirs;
            tempFN_ptr = procWalk_ptr->dirName_arr;
            while (retVal == NULL && numTries < HPROC_MAX_TRIES)
            {
                retVal = (char**)calloc(numNames + 1, sizeof(char*));
                numTries++;
            }

            if (retVal)
            {
                temp_ptr = retVal;
                
                for (i = 0; i < numNames; i++)
                {
                    if (true == is_it_a_PID((*tempFN_ptr)))
                    {
                        // fprintf(stdout, "%s is a PID!\n", *tempFN_ptr);  // DEBUGGING
                        (*temp_ptr) = copy_a_name(*tempFN_ptr);

                        if(!(*temp_ptr))
                        {
                            fprintf(stderr, "<<<ERROR>>> - Harkleproc - parse_PID_dirs_to_arr() - copy_a_name failed!\n");
                        }
                        else
                        {
                            temp_ptr++;
                        }
                    }
                    // else
                    // {
                    //     fprintf(stdout, "%s is not a PID.\n", *tempFN_ptr);  // DEBUGGING
                    // }
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


bool is_it_a_PID(const char* dirName)
{
    // LOCAL VARIABLES
    bool retVal = true;  // Default answer to prove incorrect
    const char* temp_ptr = NULL;  // Iterating variable

    // INPUT VALIDATION
    if (dirName)
    {
        if (*dirName)
        {
            temp_ptr = dirName;

            while (*temp_ptr && retVal == true)
            {
                if (*temp_ptr < 48 || *temp_ptr > 57)
                {
                    retVal = false;
                }
                else
                {
                    temp_ptr++;
                }
            }
        }
        else
        {
            retVal = false;
        }
    }

    // DONE
    return retVal;
}

