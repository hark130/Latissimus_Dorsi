#include <dirent.h>		// opendir
#include <stdlib.h>		// calloc
#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>
#include <stdlib.h>		// realloc 
#include <string.h>		// memset, strncpy

// lstat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


#ifndef HDIR_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HDIR_MAX_TRIES 3
#endif  // HDIR_MAX_TRIES

#ifndef HDIR_ARRAY_LEN
// MACRO to determinte how many indices are realloc()'d in the struct arrays
#define HDIR_ARRAY_LEN 10
#endif  // HDIR_ARRAY_LEN

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


/*
	Purpose - Coordinate the population of the fileName and dirName arrays
	Input
		updateThis_ptr - directoryDetails pointer to populate
		fileArrSize - A pointer to the actual size of the fileName_arr in memory
		dirArrSize - A pointer to the actual size of the dirName_arr in memory
	Output - true on success, false on failure
	Notes:
		This function calls populate_dirDetails_files() as appropriate
		This function calls populate_dirDetails_dirs() as appropriate
		This function does not currently support the following file types:
			Character device
			Block device
			FIFO(named pipe)
			Symbolic link
			Socket
		The array sizes may be updated as realloc() may be called
 */
bool populate_dirDetails(dirDetails_ptr updateThis_ptr);


/*
	Purpose - Coordinates array allocation and string copies of filenames 
		into a directoryDetails pointer
	Input
		updateThis_ptr - directoryDetails pointer to populate
		fileEntry - dirent struct pointer of a directory entry identified as a file
	Output - true on success, false on failure
	Notes:
		The array sizes may be updated as realloc() may be called
 */
bool populate_dirDetails_files(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry);


/*
	Purpose - Coordinates array allocation and string copies of directory names 
		into a directoryDetails pointer
	Input
		updateThis_ptr - directoryDetails pointer to populate
		fileEntry - dirent struct pointer of a directory entry identified as a directory
	Output - true on success, false on failure
	Notes:
		The array sizes may be updated as realloc() may be called
 */
bool populate_dirDetails_dirs(dirDetails_ptr updateThis_ptr, struct dirent* dirEntry);


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
	else
	{
		// ALLOCATE THE ARRAYS
		// fileName_arr
		numTries = 0;
		while(numTries < HDIR_MAX_TRIES && retVal->fileName_arr == NULL)
		{
			retVal->fileName_arr = (char**)calloc(HDIR_ARRAY_LEN + 1, sizeof(char*));
			numTries++;
	    }

	    if (!(retVal->fileName_arr))
	    {
	    	fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - Failed to allocate fileName_arr!\n");
	    }
	    else
	    {
	    	retVal->fileArrSize = (size_t)(HDIR_ARRAY_LEN + 1) * sizeof(char*);
	    }
	    // dirName_arr
		numTries = 0;
		while(numTries < HDIR_MAX_TRIES && retVal->dirName_arr == NULL)
		{
			retVal->dirName_arr = (char**)calloc(HDIR_ARRAY_LEN + 1, sizeof(char*));
			numTries++;
	    }

	    if (!(retVal->dirName_arr))
	    {
	    	fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - Failed to allocate dirName_arr!\n");
	    }
	    else
	    {
	    	retVal->dirArrSize = (size_t)(HDIR_ARRAY_LEN + 1) * sizeof(char*);
	    }
	}   

    // DONE
	return retVal;
}


dirDetails_ptr open_dir(char* directoryName)
{
	// LOCAL VARIABLES
	// 1. directoryName
	dirDetails_ptr retVal = create_dirDetails_ptr();
	char defaultDirName[] = {"."};  // cwd
	char* temp_ptr = NULL;  // Return value from strncpy()
	size_t nameLen = 0;  // Holds the length of strings to allocate
	bool popRetVal = true;  // Return value from populate()
	// 2. Files
	int fileArrSize = 0;  // Keeps track of the fileName_arr size
	// 3. Directories
	int dirArrSize = 0;  // Keeps track of the dirName_arr size

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

		// 2. Populate files & dirs
		popRetVal = populate_dirDetails(retVal);
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
	void* temp_ptr = NULL;  // Will hold the return value from memset()
	char* name_ptr = NULL;  // Will hold a char* from one of the dirDetails arrays
	int numberOfFiles = 0;  // Copy of oldStruct->numFiles
	int numberOfDirs = 0;  // Copy of oldStruct->numDirs

	// INPUT VALIDATION
	if (oldStruct_ptr)
	{
		if (*oldStruct_ptr)
		{
			oldStruct = *oldStruct_ptr;
			numberOfFiles = oldStruct->numFiles;
			numberOfDirs = oldStruct->numDirs;

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
						fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize the struct's dirName!\n");
					}

					// 1.2. free dirName
					free(oldStruct->dirName);

					// 1.3. Zero dirName
					oldStruct->dirName = NULL;
				}
			}

			// 2.a. char* fileNames;
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
								fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize one of the filenames in the struct's fileName_arr!\n");
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
			if (oldStruct->numFiles > 0)
			{
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - oldStruct->numFiles was %d, a non-zero value!\n", oldStruct->numFiles);
			}
			oldStruct->numFiles = 0;

			// 2.c. char** fileName_arr;	// Array of filenames
			// 2.c.1. memset fileName_arr
			if (oldStruct->fileArrSize >= (numberOfFiles * sizeof(char*)))
			{
				temp_ptr = memset(oldStruct->fileName_arr, 0x0, oldStruct->fileArrSize);
			}
			else
			{
				temp_ptr = memset(oldStruct->fileName_arr, 0x0, (numberOfFiles * sizeof(char*)));
			}

			if (temp_ptr != (void*)oldStruct->fileName_arr)
			{
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize the struct's fileName_arr!\n");
			}

			// 2.c.2. free fileName_arr
			free(oldStruct->fileName_arr);

			// 2.c.3. Zero fileName_arr
			oldStruct->fileName_arr = NULL;

			// 2.d. size_t fileArrSize;		// Allocated bytes for fileName_arr
			oldStruct->fileArrSize = 0;

			// 3.a. char* dirNames;
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
								fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize one of the filenames in the struct's fileName_arr!\n");
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
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - oldStruct->numDirs was %d, a non-zero value!\n", oldStruct->numDirs);
			}

			// 3.c. char** dirName_arr;		// Array of directory names
			// 3.c.1. memset dirName_arr
			if (oldStruct->dirArrSize >= (numberOfDirs * sizeof(char*)))
			{
				temp_ptr = memset(oldStruct->dirName_arr, 0x0, oldStruct->dirArrSize);
			}
			else
			{
				temp_ptr = memset(oldStruct->dirName_arr, 0x0, (numberOfDirs * sizeof(char*)));
			}

			if (temp_ptr != (void*)oldStruct->dirName_arr)
			{
				fprintf(stderr, "<<<ERROR>>> - free_dirDetails_ptr() - memset failed to zeroize the struct's dirName_arr!\n");
			}

			// 3.c.2. free dirName_arr
			free(oldStruct->dirName_arr);

			// 3.c.3. Zero dirName_arr
			oldStruct->dirName_arr = NULL;

			// 3.d. size_t dirArrSize;		// Allocated bytes for dirName_arr
			oldStruct->dirArrSize = 0;
		}
	}

	// DONE
	return retVal;
}


/*
	Purpose - Coordinate the population of the fileName and dirName arrays
	Input
		updateThis_ptr - directoryDetails pointer to populate
	Output - true on success, false on failure
	Notes:
		This function calls populate_dirDetails_files() as appropriate
		This function calls populate_dirDetails_dirs() as appropriate
		This function does not currently support the following file types:
			Character device
			Block device
			FIFO(named pipe)
			Symbolic link
			Socket
		The array sizes may be updated as realloc() may be called
 */
bool populate_dirDetails(dirDetails_ptr updateThis_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	DIR* cwd = NULL;  // Directory stream opened from dirDetails_ptr->dirName
	struct dirent* currDirEntry = NULL;  // An entry read from directory stream cwd
	int numTries = 0;  // Used to count allocation attempts

	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - updateThis_ptr NULL pointer!\n");
		retVal = false;
	}
	// else if (!(updateThis_ptr->fileArrSize))
	// {
	// 	fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - fileArrSize NULL pointer!\n");
	// 	retVal = false;
	// }
	// else if (!(updateThis_ptr->dirArrSize))
	// {
	// 	fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - dirArrSize NULL pointer!\n");
	// 	retVal = false;
	// }
	else if (updateThis_ptr->fileArrSize < 1 || updateThis_ptr->fileArrSize < (HDIR_ARRAY_LEN * sizeof(char*)))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - fileArrSize too small!\n");
		retVal = false;
	}
	else if (updateThis_ptr->dirArrSize < 1 || updateThis_ptr->dirArrSize < (HDIR_ARRAY_LEN * sizeof(char*)))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - dirArrSize too small!\n");
		retVal = false;
	}
	else if (!updateThis_ptr->dirName)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - dirName NULL pointer!\n");
		retVal = false;
	}
	else if (!(updateThis_ptr->fileName_arr))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - fileName_arr NULL pointer!\n");
		retVal = false;
	}
	else if (!(updateThis_ptr->dirName_arr))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails() - dirName_arr NULL pointer!\n");
		retVal = false;
	}

	if (retVal == true)
	{
		// WALK DIR
		// 1. Open the directory stream
		cwd = opendir(updateThis_ptr->dirName);

		// 2. Read all of the directory entries
		do
		{
			currDirEntry = readdir(cwd);

			if (currDirEntry)
			{
				switch (currDirEntry->d_type)
				{
					case DT_BLK:
						fprintf(stdout, "%s is a block device.\n", currDirEntry->d_name);
						break;
					case DT_CHR:
						fprintf(stdout, "%s is a character device.\n", currDirEntry->d_name);
						break;
					case DT_DIR:
						fprintf(stdout, "%s is a directory.\n", currDirEntry->d_name);
						break;
					case DT_FIFO:
						fprintf(stdout, "%s is a named pipe (FIFO).\n", currDirEntry->d_name);
						break;
					case DT_LNK:
						fprintf(stdout, "%s is a symbolic link.\n", currDirEntry->d_name);
						break;
					case DT_REG:
						// fprintf(stdout, "%s is a regular file.\n", currDirEntry->d_name);
						retVal = populate_dirDetails_files(updateThis_ptr, currDirEntry);
						break; 
					case DT_SOCK:
						fprintf(stdout, "%s is a UNIX domain socket.\n", currDirEntry->d_name);
						break;
					case DT_UNKNOWN:
						fprintf(stdout, "This file type is unknown.\n");
						// break;
					default:
						fprintf(stdout, "%s's file type could not be determined.\n", currDirEntry->d_name);
						fprintf(stdout, "Time to implement lstat()!\n");
						break;
				}
			}
		} while (currDirEntry);
	}

	// DONE
	return retVal;
}


bool populate_dirDetails_files(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry)
{
	// LOCAL VARIABLES
	bool retVal = true;
	// size_t currArrSize = 0;
	size_t necessarySize = 0;  // Current size + another entry + NULL terminator
	void* realloc_ptr = NULL;  // Return value from realloc
	char* temp_ptr = NULL;  // Return value from calloc

	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - updateThis_ptr NULL pointer!\n");
		retVal = false;
	}
	else if (!fileEntry)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - fileEntry NULL pointer!\n");
		retVal = false;
	}
	else if (!(updateThis_ptr->fileName_arr))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - fileEntry NULL pointer!\n");
		retVal = false;
	}

	// VALIDATE ARRAY
	// 1. Calculate necessary size
	necessarySize = updateThis_ptr->numFiles * sizeof(char*);  	// Total bytes currently stored
	necessarySize += sizeof(char*);  							// The new fileEntry to store
	necessarySize += sizeof(char*);								// The terminating NULL pointer
	// 2. Verify the array is large enough
	if (updateThis_ptr->fileArrSize < necessarySize)
	{
		// 2.1. Not enough
		realloc_ptr = realloc(updateThis_ptr->fileName_arr, updateThis_ptr->fileArrSize + HDIR_ARRAY_LEN);

		if (realloc_ptr)
		{
			updateThis_ptr->fileName_arr = realloc_ptr;
			updateThis_ptr->fileArrSize += HDIR_ARRAY_LEN;
			realloc_ptr = NULL;
		}
		else
		{
			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - failed to realloc the fileName_arr!\n");
			retVal = false;
		}
	}

	// COPY THE FILENAME
	if (retVal == true)
	{
		// 1. Allocate an array for the new filename
		temp_ptr = calloc(strlen(fileEntry->d_name), sizeof(char));

		if (temp_ptr)
		{
			// 2. Store that pointer in the struct array
			(*(updateThis_ptr->fileName_arr + updateThis_ptr->numFiles)) = temp_ptr;

			// 3. Copy the filename in
			temp_ptr = strcpy((*(updateThis_ptr->fileName_arr + updateThis_ptr->numFiles)), fileEntry->d_name);

			if (temp_ptr != (*(updateThis_ptr->fileName_arr + updateThis_ptr->numFiles)))
			{
				fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - failed to copy the d_name into the array!\n");
				retVal = false;
			}
			else
			{
				updateThis_ptr->numFiles++;  // Increment the file count
			}
		}
		else
		{
			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_files() - failed to calloc an array for the d_name!\n");
			retVal = false;
		}
	}

	// DONE
	return retVal;
}


bool populate_dirDetails_dirs(dirDetails_ptr updateThis_ptr, struct dirent* dirEntry)
{
	// LOCAL VARIABLES
	bool retVal = true;
	size_t necessarySize = 0;  // Current size + another entry + NULL terminator
	void* realloc_ptr = NULL;  // Return value from realloc
	char* temp_ptr = NULL;  // Return value from calloc

	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - updateThis_ptr NULL pointer!\n");
		retVal = false;
	}
	else if (!fileEntry)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - fileEntry NULL pointer!\n");
		retVal = false;
	}
	else if (!(updateThis_ptr->dirName_arr))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - dirName_arr NULL pointer!\n");
		retVal = false;
	}

	// VALIDATE ARRAY
	// 1. Calculate necessary size
	necessarySize = updateThis_ptr->numFiles * sizeof(char*);  	// Total bytes currently stored
	necessarySize += sizeof(char*);  							// The new fileEntry to store
	necessarySize += sizeof(char*);								// The terminating NULL pointer
	// 2. Verify the array is large enough
	if (updateThis_ptr->dirArrSize < necessarySize)
	{
		// 2.1. Not enough
		realloc_ptr = realloc(updateThis_ptr->dirName_arr, updateThis_ptr->dirArrSize + HDIR_ARRAY_LEN);

		if (realloc_ptr)
		{
			updateThis_ptr->dirName_arr = realloc_ptr;
			updateThis_ptr->dirArrSize += HDIR_ARRAY_LEN;
			realloc_ptr = NULL;
		}
		else
		{
			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to realloc the dirName_arr!\n");
			retVal = false;
		}
	}

	// COPY THE FILENAME
	if (retVal == true)
	{
		// 1. Allocate an array for the new filename
		temp_ptr = calloc(strlen(fileEntry->d_name), sizeof(char));

		if (temp_ptr)
		{
			// 2. Store that pointer in the struct array
			(*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)) = temp_ptr;

			// 3. Copy the filename in
			temp_ptr = strcpy((*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)), fileEntry->d_name);

			if (temp_ptr != (*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)))
			{
				fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to copy the d_name into the array!\n");
				retVal = false;
			}
			else
			{
				updateThis_ptr->numDirs++;  // Increment the file count
			}
		}
		else
		{
			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to calloc an array for the d_name!\n");
			retVal = false;
		}
	}

	// DONE
	return retVal;
}

	
	
