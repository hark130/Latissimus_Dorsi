#include <stdlib.h>		// calloc
#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false
#include <string.h>		// memset


#ifndef HDIR_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HDIR_MAX_TRIES 3
#endif // HDIR_MAX_TRIES

/*
typedef struct directoryDetails
{
	char* dirName;
	int numFiles;
	char** fileName_arr;
	int numDirs;
	char** dirName_arr;
} dirDetails, *dirDetails_ptr;
 */


dirDetails_ptr create_dirDetails_ptr(void)
{
    // LOCAL VARIABLES
    dirDetails_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while(numTries < HDIR_MAX_TRIES && retVal == NULL)
	{
		retVal = (dirDetails_ptr)calloc(1, sizeof(dirDetails));
		numTries++;
    }
    
    // DONE
	return retVal;
}


/*
	Purpose - Read and parse a given directory name
	Input
		dirName - stack-allocated, nul-terminated, directory name
	Output - heap-allocated, fully populated, directoryDetails struct pointer
	Notes:
		
 */
dirDetails_ptr open_dir(char* dirName);


/*
	Purpose - Zeroize, nullify, and free a heap-allocated directoryDetails struct pointer
	Input
		oldStruct_ptr - Pointer to a dirDetails_ptr
	Output - true on success, false on failure
	Notes:
		Will memset(0x0), free, and NULL any pointers
		Will zeroize all other members
 */
bool free_dirDetails_ptr(dirDetails_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	dirDetails_ptr oldStruct = NULL;
	char* temp_ptr = NULL;

	// INPUT VALIDATION
	if (oldStruct_ptr)
	{
		if (*oldStruct_ptr)
		{
			oldStruct = *oldStruct_ptr;

			// FREE
			// 1. char* dirName;
			if (oldStruct->dirName)
			{
				if (*(oldStruct->dirname))
				{
					// 1.1. memset dirName
					temp_ptr = memset(oldStruct->dirName, 0x0, strlen(oldStruct->dirName));

					if (temp_ptr != oldStruct->dirName)
					{
						fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize the struct's dirName\n");
					}

					// 1.2. free dirName
					free(oldStruct->dirName);

					// 1.3. Zero dirName
					oldStruct->dirName = NULL;
				}
			}

			// 2. int numFiles;
			oldStruct->numFiles = 0;

			// char** fileName_arr;
			// int numDirs;
			// char** dirName_arr;
		}
	}

	// DONE
	return retVal;
}

/*
typedef struct directoryDetails
{
	char* dirName;
	int numFiles;
	char** fileName_arr;
	int numDirs;
	char** dirName_arr;
} dirDetails, *dirDetails_ptr;
 */