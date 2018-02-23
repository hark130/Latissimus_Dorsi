#include <stdlib.h>		// calloc
#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>
#include <string.h>		// memset, strncpy


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

    if (!retVal)
    {
    	fprintf(stderr, "<<<ERROR>>> - create_dirDetails_ptr() - Failed to allocate a dirDetails struct pointer!\n");
    }
    
    // DONE
	return retVal;
}


dirDetails_ptr open_dir(char* directoryName)
{
	// LOCAL VARIABLES
	dirDetails_ptr retVal = create_dirDetails_ptr();
	char defaultDirName[] = {"."};  // cwd
	char* temp_ptr = NULL;  // Return value from strncpy()
	size_t nameLen = 0;  // Holds the length of strings to allocate

	if (retVal)
	{
		// POPULATE dirDetails_ptr
		// 1. Copy in directoryName
		// 1.1. Validate directoryName
		if (!directoryName)
		{
			directoryName = defaultDirName;
		}
		else
		{
			if (*directoryName == 0)
			{
				directoryName = defaultDirName;
			}
		}

		// 1.2. Allocate a char array
		nameLen = strlen(directoryName);
		retVal->dirName = (char*)calloc(nameLen + 1, sizeof(char));

		if (!(retVal->dirName))
		{
			fprintf(stderr, "<<<ERROR>>> - open_dir() - Failed to allocate an array for dirName!\n");
		}
		else
		{
			temp_ptr = strncpy(retVal->dirName, directoryName, nameLen);

			if (temp_ptr != retVal->dirName)
			{
				fprintf(stderr, "<<<ERROR>>> - open_dir() - Failed to copy the directory name into dirName!\n");
			}
		}

		// 2. Populate files
		///////////////////////////////////// IMPLEMENT LATER /////////////////////////////////////

		// 3. Populate dirs
		///////////////////////////////////// IMPLEMENT LATER /////////////////////////////////////
	}

	// DONE
	return retVal;
}


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
	dirDetails_ptr oldStruct = NULL;  // Easier to deal with the oldStruct this way
	char* temp_ptr = NULL;  // Will hold the return value from memset()
	char* name_ptr = NULL;  // Will hold a char* from one of the dirDetails arrays

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
				if (*(oldStruct->dirName))
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

			// 2.a. char** fileName_arr;
			if (oldStruct->numFiles > 0)
			{
				while (oldStruct->numFiles >= 0)
				{
					name_ptr = (*(oldStruct->fileName_arr + oldStruct->numFiles));

					if (name_ptr)
					{
						if (*name_ptr)
						{
							// 2.a.1. memset filename
							temp_ptr = memset(name_ptr, 0x0, strlen(name_ptr));

							if (temp_ptr != name_ptr)
							{
								fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize one of the filenames in the struct's fileName_arr\n");
							}
						}

					}

					// 2.a.2. free filename
					free(name_ptr);

					// 2.a.3. Zero filename
					name_ptr = NULL;
					(*(oldStruct->fileName_arr + oldStruct->numFiles)) = NULL;

					// 2.a.4. Next filename
					oldStruct->numFiles--;
				}
			}

			// 2.b. int numFiles;
			if (oldStruct->numFiles != 0)
			{
				oldStruct->numFiles = 0;
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - oldStruct->numFiles was %d, a non-zero value\n", oldStruct->numFiles);
			}
			

			// 3.a. char** dirName_arr;
			if (oldStruct->numDirs > 0)
			{
				while (oldStruct->numDirs >= 0)
				{
					name_ptr = (*(oldStruct->dirName_arr + oldStruct->numDirs));

					if (name_ptr)
					{
						if (*name_ptr)
						{
							// 3.a.1. memset filename
							temp_ptr = memset(name_ptr, 0x0, strlen(name_ptr));

							if (temp_ptr != name_ptr)
							{
								fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize one of the filenames in the struct's fileName_arr\n");
							}
						}

					}

					// 3.a.2. free filename
					free(name_ptr);

					// 3.a.3. Zero filename
					name_ptr = NULL;
					(*(oldStruct->dirName_arr + oldStruct->numDirs)) = NULL;

					// 3.a.4. Next filename
					oldStruct->numDirs--;
				}
			}

			// 3.b. int numDirs;
			if (oldStruct->numDirs != 0)
			{
				oldStruct->numDirs = 0;
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - oldStruct->numDirs was %d, a non-zero value\n", oldStruct->numDirs);
			}
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