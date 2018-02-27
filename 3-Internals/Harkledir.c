#include <dirent.h>		// opendir
#include "Fileroad.h"	// size_a_file
#include "Harkledir.h"
#include <limits.h>		// UCHAR_MAX
#include "Memoroad.h"	// release_a_string
#include <stdbool.h>	// bool, true, false
#include <stdio.h>
#include <stdlib.h>		// calloc, realloc 
#include <string.h>		// memset, strncpy
#include <sys/types.h>	// ino_t
#include <unistd.h>		// readlink

#ifndef HDIR_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HDIR_MAX_TRIES 3
#endif  // HDIR_MAX_TRIES

#ifndef HDIR_ARRAY_LEN
// MACRO to determinte how many indices are realloc()'d in the struct arrays
#define HDIR_ARRAY_LEN 10
#endif  // HDIR_ARRAY_LEN

#ifndef HDIR_MEMSET_DEFAULT
#define HDIR_MEMSET_DEFAULT 0x0
#endif  // MEMSET_DEFAULT

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif // HARKLE_ERROR

//////////////////////////////////////////////////////////////////////////////
/////////////////////// LOCAL FUNCTION PROTOTYPES START //////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Coordinate the population of the fileName and dirName arrays
	Input
		updateThis_ptr - directoryDetails pointer to populate
		fileArrSize - A pointer to the actual size of the fileName_arr in memory
		dirArrSize - A pointer to the actual size of the dirName_arr in memory
	Output - true on success, false on failure
	Notes:
		This function calls populate_dirDetails_arrays()
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
bool populate_dirDetails_arrays(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry);


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
// bool populate_dirDetails_dirs(dirDetails_ptr updateThis_ptr, struct dirent* dirEntry);


//////////////////////////////////////////////////////////////////////////////
/////////////////////// LOCAL FUNCTION PROTOTYPES STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////// HARKLEDIRENT FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


hdEnt_ptr create_hdEnt_ptr(void)
{
	// LOCAL VARIABLES
	hdEnt_ptr retVal = NULL;
	int numTries = 0;  // Tracks allocation attempts
	
	// ALLOCATION
	while (!retVal && numTries < HDIR_MAX_TRIES)
	{
		retVal = (hdEnt_ptr)calloc(1, sizeof(hdEnt));
		numTries++;
	}
	
	if (!retVal)
	{
		HARKLE_ERROR(Harkledir, create_hdEnt_ptr, calloc failed);
	}	
	
	// DONE
	return retVal;
}


bool populate_hdEnt_struct(hdEnt_ptr updateThis_ptr, struct dirent* currDirEntry)
{
	// LOCAL VARIABLES
	bool retVal = true;
	off_t symLinkLength = 0;  // Will be used to allocate an array for any symlinks
	ssize_t numBytesRead = 0;  // Return 
	
	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		HARKLE_ERROR(Harkledir, populate_hdEnt_struct, NULL pointer);
		retVal = false;
	}
	else if (!currDirEntry)
	{
		HARKLE_ERROR(Harkledir, populate_hdEnt_struct, NULL pointer);
		retVal = false;
	}
	
	// POPULATE STRUCT
	if (retVal == true)
	{
		// char* hd_Name;				// Should match struct dirent.d_name
		updateThis_ptr->hd_Name = copy_a_string(currDirEntry->d_name);
		
		if (!(updateThis_ptr->hd_Name))
		{
			HARKLE_ERROR(Harkledir, populate_hdEnt_struct, copy_a_string failed);
			retVal = false;
		}
		
		// ino_t hd_inodeNum;			// Should match struct dirent.d_ino
		switch (currDirEntry->d_type)
		{
			case DT_UNKNOWN:
				fprintf(stdout, "This file type is unknown.\n");
			case DT_BLK:
			case DT_CHR:
			case DT_DIR:
			case DT_FIFO:
			case DT_LNK:
			case DT_REG:
			case DT_SOCK:
				updateThis_ptr->hd_inodeNum = currDirEntry->d_ino;
				break;
			default:
				fprintf(stdout, "%s's file type could not be determined.\n", currDirEntry->d_name);
				fprintf(stdout, "Time to implement lstat()!\n");
				retVal = false;
				break;
		}
		
		// unsigned char hd_type; 		// Should match struct dirent.d_type
		updateThis_ptr->hd_type = currDirEntry->d_type;
		
		// char* hd_symName; 			// If hd_type == DT_LNK, read from readlink()
		if (retVal == true && updateThis_ptr->hd_inodeNum == DT_LNK)
		{
			symLinkLength = size_a_file(updateThis_ptr->hd_Name);
			
			if (symLinkLength == -1)
			{
				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed);
				retVal = false;
			}
			else if (symLinkLength == 0)
			{
				// Some symlinks (e.g., /proc, /sys) report 'st_size' as zero.
				// PATH_MAX should be a "good enough" estimate
				symLinkLength = PATH_MAX;
			}
			
			if (retVal == true)
			{
				// Increase the size by one to look for truncation
				symLinkLength++;
				
				// Allocate a buffer
				updateThis_ptr->hd_symName = get_me_a_buffer(symLinkLength + 1);
				
				if (!(updateThis_ptr->hd_symName))
				{
					HARKLE_ERROR(Harkledir, populate_hdEnt_struct, get_me_a_buffer failed);
					retVal = false;
				}
				else
				{
					// Read into that buffer
					numBytesRead = readlink(updateThis_ptr->hd_Name, updateThis_ptr->hd_symName, symLinkLength);

					// Validate the read
					if (numBytesRead == -1)
					{
						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed);
						retVal = false;
					}
					else if (numBytesRead == symLinkLength)
					{
						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, Buffer read may have been truncated);
					}
				}				
			}			
		}
	}
	
	// DONE
	return retVal;
}


bool free_hdEnt_ptr(hdEnt_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Make this false if anything fails
	hdEnt_ptr freeThis_ptr = NULL;  // Will be assigned *oldStruct_ptr if input validation passes
	bool mrRetVal = true;  // Will hold return value form Memoroad function calls
	char* temp_ptr = NULL;  // Used to assist in free()ing strings
	
	// INPUT VALIDATION
	if (oldStruct_ptr == NULL)
	{
		HARKLE_ERROR(Harkledir, free_hdEnt_ptr, NULL pointer);
		retVal = false;	
	}
	else if (*oldStruct_ptr == NULL)
	{
		HARKLE_ERROR(Harkledir, free_hdEnt_ptr, NULL pointer);
		retVal = false;	
	}
	else
	{
		freeThis_ptr = *oldStruct_ptr;	
	}
	
	// FREE
	if (retVal == true)
	{
		// 1. Memset/Free/NULL the struct contents
		// char* hd_Name;				// Should match struct dirent.d_name
		temp_ptr = freeThis_ptr->hd_Name;
		// fprintf(stdout, "free_hdEnt_ptr() - About to free() hd_Name %s (%p)\n", temp_ptr, temp_ptr);
		mrRetVal = release_a_string(&temp_ptr);
		// mrRetVal = release_a_string(&(freeThis_ptr->hd_Name));
		
		if (mrRetVal == false)
		{
			HARKLE_ERROR(Harkledir, free_hdEnt_ptr, release_a_string failed);
			retVal = false;	
		}
		
		// ino_t hd_inodeNum;			// Should match struct dirent.d_ino
		freeThis_ptr->hd_inodeNum = 0;
		
		// unsigned char hd_type; 		// Should match struct dirent.d_type
		freeThis_ptr->hd_type = 0;
		
		if (freeThis_ptr->hd_symName)
		{
			// char* hd_symName;			// If hd_type == DT_LNK, read from readlink()
			temp_ptr = freeThis_ptr->hd_symName;
			// fprintf(stdout, "free_hdEnt_ptr() - About to free() hd_symName %s (%p)\n", temp_ptr, temp_ptr);
			mrRetVal = release_a_string(&temp_ptr);
			// mrRetVal = release_a_string(&(freeThis_ptr->hd_symName));
		
			if (mrRetVal == false)
			{
				HARKLE_ERROR(Harkledir, free_hdEnt_ptr, release_a_string failed);
				retVal = false;	
			}
		}
		
		// 2. Free the struct pointer
		// fprintf(stdout, "free_hdEnt_ptr() - About to free() %p\n", *oldStruct_ptr);
		free(*oldStruct_ptr);
		
		
		// 3. NULL this pointer
		freeThis_ptr = NULL;
		*oldStruct_ptr = NULL;
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////// HARKLEDIRENT FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////// DIRECTORYDETAILS FUNCTIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////


dirDetails_ptr create_dirDetails_ptr(void)
{
    // LOCAL VARIABLES
    dirDetails_ptr retVal = NULL;
	int numTries = 0;
	bool success = true;  // If anything fails, make this false

	// ALLOCATE MEMORY
	while(numTries < HDIR_MAX_TRIES && retVal == NULL)
	{
		retVal = (dirDetails_ptr)calloc(1, sizeof(dirDetails));
		numTries++;
    }

    if (!retVal)
    {
		HARKLE_ERROR(Harkledir, create_dirDetails_ptr, dirDetails_ptr calloc failed);
		success = false;
    }
	else
	{
		// ALLOCATE THE ARRAYS
		// fileName_arr
		numTries = 0;
		while(numTries < HDIR_MAX_TRIES && retVal->fileName_arr == NULL)
		{
			retVal->fileName_arr = (hdEnt_ptr*)calloc(HDIR_ARRAY_LEN + 1, sizeof(hdEnt_ptr));
			numTries++;
	    }

	    if (!(retVal->fileName_arr))
	    {
			HARKLE_ERROR(Harkledir, create_dirDetails_ptr, fileName_arr calloc failed);
			success = false;
	    }
	    else
	    {
	    	retVal->fileArrSize = (size_t)(HDIR_ARRAY_LEN + 1) * sizeof(hdEnt_ptr);
	    }
	    // dirName_arr
		numTries = 0;
		while(numTries < HDIR_MAX_TRIES && retVal->dirName_arr == NULL)
		{
			retVal->dirName_arr = (hdEnt_ptr*)calloc(HDIR_ARRAY_LEN + 1, sizeof(hdEnt_ptr));
			numTries++;
	    }

	    if (!(retVal->dirName_arr))
	    {
			HARKLE_ERROR(Harkledir, create_dirDetails_ptr, dirName_arr calloc failed);
			success = false;
	    }
	    else
	    {
	    	retVal->dirArrSize = (size_t)(HDIR_ARRAY_LEN + 1) * sizeof(hdEnt_ptr);
	    }
	}
	
	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_dirDetails_ptr(&retVal))
			{
				HARKLE_ERROR(Harkledir, create_dirDetails_ptr, free_dirDetails_ptr failed);
			}
		}
	}

    // DONE
	return retVal;
}


dirDetails_ptr open_dir(char* directoryName)
{
	// LOCAL VARIABLES
	bool success = true;  // Set this to false if anything fails
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
		// nameLen = strlen(directoryName);
		// retVal->dirName = (char*)calloc(nameLen + 1, sizeof(char));

		// if (!(retVal->dirName))
		// {
		// 	HARKLE_ERROR(Harkledir, open_dir, dirName calloc failed);
		// 	success = false;
		// }
		// else
		// {
		// 	temp_ptr = strncpy(retVal->dirName, directoryName, nameLen);

		// 	if (temp_ptr != retVal->dirName)
		// 	{
		// 		HARKLE_ERROR(Harkledir, open_dir, dirName strncpy failed);
		// 		success = false;
		// 	}
		// }
		retVal->dirName = copy_a_string(directoryName);

		if (!(retVal->dirName))
		{
			HARKLE_ERROR(Harkledir, open_dir, copy_a_string failed);
			success = false;
		}

		// 2. Populate files & dirs
		if (success == true)
		{
			popRetVal = populate_dirDetails(retVal);

			if (popRetVal == false)
			{
				HARKLE_ERROR(Harkledir, open_dir, populate_dirDetails failed);
				success = false;
			}
		}
	}
	else
	{
		HARKLE_ERROR(Harkledir, open_dir, create_dirDetails_ptr failed);
		success = false;
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_dirDetails_ptr(&retVal))
			{
				HARKLE_ERROR(Harkledir, open_dir, free_dirDetails_ptr failed);
			}
		}
	}

	// DONE
	return retVal;
}


bool free_dirDetails_ptr(dirDetails_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	dirDetails_ptr oldStruct = NULL;  // Easier to deal with the oldStruct this way
	void* temp_ptr = NULL;  // Will hold the return value from memset()
	hdEnt* hDE_ptr = NULL;  // Will hold a harkleDirEnt* from one of the dirDetails arrays
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
			// puts("1. char* dirName");  // DEBUGGING
			// 1. char* dirName;
			if (oldStruct->dirName)
			{
				if (*(oldStruct->dirName))
				{
					if (false == release_a_string(&(oldStruct->dirName)))
					{
						HARKLE_ERROR(Harkledir, free_dirDetails_ptr, release_a_string failed);
						retVal = false;
					}
				}
			}

			// 2. File Array
			// puts("2. File Array");  // DEBUGGING
			// 2.a. fileName_arr and all hdEnt_ptr contained within
			// puts("2.a. fileName_arr and all hdEnt_ptr contained within");  // DEBUGGING
			if (oldStruct->numFiles > 0)
			{
				while (numberOfFiles > 0)
				{
					// 2.a.1. hdEnt_ptr	
					// puts("2.a.1. hdEnt_ptr");  // DEBUGGING
					hDE_ptr = (hdEnt_ptr)(*(oldStruct->fileName_arr + numberOfFiles - 1));

					if (hDE_ptr)
					{
						if (false == free_hdEnt_ptr(&hDE_ptr))
						{
							HARKLE_ERROR(Harkledir, free_dirDetails_ptr, free_hdEnt_ptr failed);
							retVal = false;
						}
						else
						{
							(*(oldStruct->fileName_arr + numberOfFiles - 1)) = NULL;
						}
					}
					else
					{
						HARKLE_ERROR(Harkledir, free_dirDetails_ptr, NULL pointer in fileName_arr);
						retVal = false;
					}

					// 2.a.2. Next struct
					// puts("2.a.2. Next struct");  // DEBUGGING
					numberOfFiles--;
				}
			}

			// 2.a.3. fileName_arr
			// puts("2.a.3. fileName_arr");  // DEBUGGING
			// 2.a.3.i. memset
			if (oldStruct->fileArrSize > 0)
			{
				temp_ptr = memset(oldStruct->fileName_arr, HDIR_MEMSET_DEFAULT, oldStruct->fileArrSize);

				if (temp_ptr != oldStruct->fileName_arr)
				{
					HARKLE_ERROR(Harkledir, free_dirDetails_ptr, memset failed on fileName_arr);
					retVal = false;
				}
			}
			// 2.a.3.ii. free
			free(oldStruct->fileName_arr);
			// 2.a.3.iii. NULL
			temp_ptr = NULL;
			oldStruct->fileName_arr = NULL;

			// 2.b. int numFiles;
			if (numberOfFiles > 0)
			{
				HARKLE_ERROR(Harkledir, free_dirDetails_ptr, numberOfFiles was a non-zero value);
				retVal = false;
			}
			oldStruct->numFiles = 0;

			// 2.c. size_t fileArrSize;		// Allocated bytes for fileName_arr
			oldStruct->fileArrSize = 0;

			// 3. Directory Array;
			// 3.a. dirName_arr and all hdEnt_ptr contained within
			if (oldStruct->numDirs > 0)
			{
				while (numberOfDirs > 0)
				{
					// 3.a.1. hdEnt_ptr	
					hDE_ptr = (hdEnt*)(*(oldStruct->dirName_arr + numberOfDirs - 1));

					if (hDE_ptr)
					{
						if (false == free_hdEnt_ptr(&hDE_ptr))
						{
							HARKLE_ERROR(Harkledir, free_dirDetails_ptr, free_hdEnt_ptr failed);
							retVal = false;
						}
						else
						{
							(*(oldStruct->dirName_arr + numberOfDirs - 1)) = NULL;
						}
					}
					else
					{
						HARKLE_ERROR(Harkledir, free_dirDetails_ptr, NULL pointer in dirName_arr);
						retVal = false;
					}

					// 3.a.2. Next struct
					numberOfDirs--;
				}
			}

			// 3.a.3. dirName_arr
			// 3.a.3.i. memset
			if (oldStruct->dirArrSize > 0)
			{
				temp_ptr = memset(oldStruct->dirName_arr, HDIR_MEMSET_DEFAULT, oldStruct->dirArrSize);

				if (temp_ptr != oldStruct->dirName_arr)
				{
					HARKLE_ERROR(Harkledir, free_dirDetails_ptr, memset failed on dirName_arr);
					retVal = false;
				}
			}
			// 3.a.3.ii. free
			free(oldStruct->dirName_arr);
			// 3.a.3.iii. NULL
			temp_ptr = NULL;
			oldStruct->dirName_arr = NULL;

			// 3.b. int numDirs;
			if (numberOfDirs > 0)
			{
				HARKLE_ERROR(Harkledir, free_dirDetails_ptr, numberOfDirs was a non-zero value);
				retVal = false;
			}
			oldStruct->numDirs = 0;

			// 3.c. size_t dirArrSize;		// Allocated bytes for fileName_arr
			oldStruct->dirArrSize = 0;
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
////////////////////// DIRECTORYDETAILS FUNCTIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////


bool populate_dirDetails(dirDetails_ptr updateThis_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	DIR* cwd = NULL;  // Directory stream opened from dirDetails_ptr->dirName
	struct dirent* currDirEntry = NULL;  // An entry read from directory stream cwd
	int numTries = 0;  // Used to count allocation attempts
	unsigned char fileType = 0;  // Return value from get_a_file_type()
	char* absPath_ptr = NULL;  // Just in case we build an absolute path

	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, updateThis_ptr NULL pointer);
		retVal = false;
	}
	else if (updateThis_ptr->fileArrSize < 1 || updateThis_ptr->fileArrSize < (HDIR_ARRAY_LEN * sizeof(hdEnt_ptr)))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, fileArrSize too small);
		retVal = false;
	}
	else if (updateThis_ptr->dirArrSize < 1 || updateThis_ptr->dirArrSize < (HDIR_ARRAY_LEN * sizeof(hdEnt_ptr)))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, dirArrSize too small);
		retVal = false;
	}
	else if (!updateThis_ptr->dirName)
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, dirName NULL pointer);
		retVal = false;
	}
	else if (!(updateThis_ptr->fileName_arr))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, fileName_arr NULL pointer);
		retVal = false;
	}
	else if (!(updateThis_ptr->dirName_arr))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails, dirName_arr NULL pointer);
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
				// fprintf(stdout, "currDirEntry->d_name == %s\n", currDirEntry->d_name);  // DEBUGGING
				// fprintf(stdout, "currDirEntry->d_type == %u\n", currDirEntry->d_type);  // DEBUGGING

				switch (currDirEntry->d_type)
				{
					case DT_LNK:
					case DT_REG:
					case DT_DIR:
					case DT_FIFO:
					case DT_SOCK:
					case DT_BLK:
					case DT_CHR:
						break;
					case DT_UNKNOWN:
					default:
						// fprintf(stdout, "populate_dirDetails() found an invalid d_type of %u.\nGetting a second opinion from get_a_file_type().\n", currDirEntry->d_type);  // DEBUGGING
						absPath_ptr = os_path_join(updateThis_ptr->dirName, currDirEntry->d_name, false);

						if (absPath_ptr)
						{
							fileType = get_a_file_type(absPath_ptr);

							if (fileType == UCHAR_MAX)
							{
								HARKLE_ERROR(Harkledir, populate_dirDetails, get_a_file_type failed);
								retVal = false;
							}
							else
							{
								// SUCCESS!
								// fprintf(stdout, "get_a_file_type() returned %u.\n", fileType);
								currDirEntry->d_type = fileType;
							}
						}
						else
						{
							HARKLE_ERROR(Harkledir, populate_dirDetails, os_path_join failed);
							retVal = false;
						}
						break;
				}
				retVal = populate_dirDetails_arrays(updateThis_ptr, currDirEntry);
			}
		} while (currDirEntry && retVal == true);
	}

	// CLEAN UP
	if (absPath_ptr)
	{
		if (false == release_a_string(&absPath_ptr))
		{
			HARKLE_ERROR(Harkledir, populate_dirDetails, release_a_string failed);
		}
	}

	// DONE
	return retVal;
}


bool populate_dirDetails_arrays(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry)
{
	// LOCAL VARIABLES
	bool retVal = true;
	size_t necessarySize = 0;  // Current size + another entry + NULL terminator
	void* realloc_ptr = NULL;  // Return value from realloc
	char* temp_ptr = NULL;  // Return value from calloc
	hdEnt_ptr newStruct = NULL;  // Will temporarily hold new struct pointer allocation
	// Abstract dirDetails References
	int* numEntries_ptr = NULL;  // Pointer to the numThings member of the relevant section
	hdEnt_ptr** abstractArr_ptr = NULL;  // Pointer to the array member of the relevant section
	size_t* arraySize_ptr = NULL;  // Pointer to the arraySize member of the relevant section
	// hdEnt_ptr** debug_ptr = NULL;  // Pointer to the array member of the relevant section


	// INPUT VALIDATION
	if (!updateThis_ptr)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_arrays() - updateThis_ptr NULL pointer!\n");
		retVal = false;
	}
	else if (!fileEntry)
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_arrays() - fileEntry NULL pointer!\n");
		retVal = false;
	}
	else if (!(updateThis_ptr->fileName_arr))
	{
		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_arrays() - fileEntry NULL pointer!\n");
		retVal = false;
	}

	// DECIDE WHICH ARRAY TO UPDATE
	if (retVal == true)
	{
		// fprintf(stdout, "fileEntry->d_name == %s\n", fileEntry->d_name);  // DEBUGGING
		// fprintf(stdout, "fileEntry->d_ino == %lu\n", fileEntry->d_ino);  // DEBUGGING
		switch (fileEntry->d_type)
		{
			case DT_LNK:
			case DT_REG:
				numEntries_ptr = &(updateThis_ptr->numFiles);
				abstractArr_ptr = &(updateThis_ptr->fileName_arr);
				arraySize_ptr = &(updateThis_ptr->fileArrSize);
				break;
			case DT_DIR:
				numEntries_ptr = &(updateThis_ptr->numDirs);
				abstractArr_ptr = &(updateThis_ptr->dirName_arr);
				arraySize_ptr = &(updateThis_ptr->dirArrSize);
				break;
			case DT_FIFO:
			case DT_SOCK:
			case DT_BLK:
			case DT_CHR:
			case DT_UNKNOWN:
				break;
			default:
				HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, Invalid dirent data type);
				retVal = false;
				break;
		}
	}

	// VALIDATE ARRAY
	if (retVal == true && numEntries_ptr && abstractArr_ptr && arraySize_ptr)
	{
		// 1. Calculate necessary size
		necessarySize = (*numEntries_ptr) * sizeof(hdEnt_ptr); 		 	// Total bytes currently stored
		necessarySize += sizeof(hdEnt_ptr);  							// The new hdEntry to store
		necessarySize += sizeof(hdEnt_ptr);								// The terminating NULL pointer
		// 2. Verify the array is large enough
		if (*arraySize_ptr < necessarySize)
		{
			// 2.1. Not enough
			// fprintf(stdout, "\n\npopulate_dirDetails_arrays() - Array %p not big enough at %zu bytes.\n", *abstractArr_ptr, (*arraySize_ptr));  // DEBUGGING
			realloc_ptr = realloc(*abstractArr_ptr, (*arraySize_ptr) + (HDIR_ARRAY_LEN * sizeof(hdEnt_ptr)));
			// /* DEBUGGING */
			// debug_ptr = abstractArr_ptr;
			// puts("BEFORE:");
			// while (*debug_ptr != NULL)
			// {
			// 	fprintf(stdout, "%p, ", *debug_ptr);
			// 	debug_ptr++;
			// }
			// putchar(0xA);

			if (realloc_ptr)
			{
				*abstractArr_ptr = realloc_ptr;
				*arraySize_ptr += (HDIR_ARRAY_LEN * sizeof(hdEnt_ptr));
				realloc_ptr = NULL;
				// Set the last index to NULL
				// (*((*abstractArr_ptr) + (*arraySize_ptr) - 1)) = NULL;
				// fprintf(stdout, "populate_dirDetails_arrays() - Array %p now big enough with %zu bytes.\n", *abstractArr_ptr, (*arraySize_ptr));  // DEBUGGING
			}
			else
			{
				HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, Failed to realloc the fileName_arr);
				retVal = false;
			}

			// /* DEBUGGING */
			// debug_ptr = abstractArr_ptr;
			// puts("AFTER:");
			// while (*debug_ptr != NULL)
			// {
			// 	fprintf(stdout, "%p, ", *debug_ptr);
			// 	debug_ptr++;
			// }
			// putchar(0xA);
		}
	}

	// POPULATE THE ARRAY
	if (retVal == true && numEntries_ptr && abstractArr_ptr && arraySize_ptr)
	{
		// 1. Allocate a struct
		newStruct = create_hdEnt_ptr();

		if (newStruct)
		{
			// 2. Populate that struct
			if (true == populate_hdEnt_struct(newStruct, fileEntry))
			{
				// 3. Store that pointer in the struct array
				*((*abstractArr_ptr) + (*numEntries_ptr)) = newStruct;
				newStruct = NULL;

				// 4. Increment the file count
				(*numEntries_ptr)++;
			}
			else
			{
				HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, populate_hdEnt_struct failed);
				retVal = false;
			}
		}
		else
		{
			HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, copy_a_string failed);
			retVal = false;
		}
	}

	// DONE
	return retVal;
}


// bool populate_dirDetails_dirs(dirDetails_ptr updateThis_ptr, struct dirent* dirEntry)
// {
// 	// LOCAL VARIABLES
// 	bool retVal = true;
// 	size_t necessarySize = 0;  // Current size + another entry + NULL terminator
// 	void* realloc_ptr = NULL;  // Return value from realloc
// 	char* temp_ptr = NULL;  // Return value from calloc

// 	// INPUT VALIDATION
// 	if (!updateThis_ptr)
// 	{
// 		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - updateThis_ptr NULL pointer!\n");
// 		retVal = false;
// 	}
// 	else if (!dirEntry)
// 	{
// 		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - fileEntry NULL pointer!\n");
// 		retVal = false;
// 	}
// 	else if (!(updateThis_ptr->dirName_arr))
// 	{
// 		fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - dirName_arr NULL pointer!\n");
// 		retVal = false;
// 	}

// 	// VALIDATE ARRAY
// 	// 1. Calculate necessary size
// 	necessarySize = updateThis_ptr->numDirs * sizeof(char*);  	// Total bytes currently stored
// 	necessarySize += sizeof(char*);  							// The new fileEntry to store
// 	necessarySize += sizeof(char*);								// The terminating NULL pointer
// 	// 2. Verify the array is large enough
// 	if (updateThis_ptr->dirArrSize < necessarySize)
// 	{
// 		// 2.1. Not enough
// 		realloc_ptr = realloc(updateThis_ptr->dirName_arr, updateThis_ptr->dirArrSize + HDIR_ARRAY_LEN);

// 		if (realloc_ptr)
// 		{
// 			updateThis_ptr->dirName_arr = realloc_ptr;
// 			updateThis_ptr->dirArrSize += HDIR_ARRAY_LEN;
// 			realloc_ptr = NULL;
// 			// Set the last index to NULL
// 			(*(updateThis_ptr->dirName_arr + updateThis_ptr->dirArrSize - 1)) = NULL;
// 		}
// 		else
// 		{
// 			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to realloc the dirName_arr!\n");
// 			retVal = false;
// 		}
// 	}

// 	// COPY THE FILENAME
// 	if (retVal == true)
// 	{
// 		// 1. Allocate an array for the new filename
// 		temp_ptr = calloc(strlen(dirEntry->d_name), sizeof(char));

// 		if (temp_ptr)
// 		{
// 			// 2. Store that pointer in the struct array
// 			(*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)) = temp_ptr;

// 			// 3. Copy the filename in
// 			temp_ptr = strcpy((*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)), dirEntry->d_name);

// 			if (temp_ptr != (*(updateThis_ptr->dirName_arr + updateThis_ptr->numDirs)))
// 			{
// 				fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to copy the d_name into the array!\n");
// 				retVal = false;
// 			}
// 			else
// 			{
// 				updateThis_ptr->numDirs++;  // Increment the file count
// 			}
// 		}
// 		else
// 		{
// 			fprintf(stderr, "<<<ERROR>>> - populate_dirDetails_dirs() - failed to calloc an array for the d_name!\n");
// 			retVal = false;
// 		}
// 	}

// 	// DONE
// 	return retVal;
// }

	
//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////


