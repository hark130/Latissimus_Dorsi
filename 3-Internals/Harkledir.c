#include <dirent.h>		// opendir
#include <errno.h>
#include "Fileroad.h"	// size_a_file
#include "Harkledir.h"
#include <inttypes.h>	// intmax_t
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

#ifndef HDIR_BIG_BUFF_SIZE
// MACRO to standardize large buffer allocation
#define HDIR_BIG_BUFF_SIZE 4096
#endif  // HDIR_BIG_BUFF_SIZE

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
		absPath - nul-terminated absolute path to the file at *currDirEntry
	Output - true on success, false on failure
	Notes:
		The array sizes may be updated as realloc() may be called
 */
bool populate_dirDetails_arrays(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry, char* absPath);


/*
	Purpose - As a helper function to parse_dirDetails_to_char_arr(), compare
		a hd_type and typeFlags to determine if this file should be included
	Input
		hdEntryType - harkleDirEnt's hd_type to evaluate for inclusion
		typeFlags - Reference to Harkledir.h "typeFlags" MACRO FLAGS
	Output
		True if it matches
		False if there's no match
	Notes:
		This helper function was written expressly for 
			parse_dirDetails_to_char_arr() and it may never be useful
			elsewhere
 */
bool keep_hdEntry(unsigned char hdEntryType, unsigned int typeFlags);


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


bool populate_hdEnt_struct(hdEnt_ptr updateThis_ptr, struct dirent* currDirEntry, char* absPath)
{
	// LOCAL VARIABLES
	bool retVal = true;
	off_t symLinkLength = 0;  // Will be used to allocate an array for any symlinks
	ssize_t numBytesRead = 0;  // Return
	// char* absPath_ptr = NULL;  // Should hold absolute path for an file passed to size_a_file()
	bool isThisAFile = true;  // Third parameter for os_path_join()
	int errNum = 0;  // Pass as an [OUT] parameter to size_a_file() to get errno
	
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
	else if (!absPath)
	{
		HARKLE_ERROR(Harkledir, populate_hdEnt_struct, NULL pointer);
		retVal = false;
	}
	else if (!(*absPath))
	{
		HARKLE_ERROR(Harkledir, populate_hdEnt_struct, Empty path);
		retVal = false;
	}
	
	// POPULATE STRUCT
	// char* hd_Name;				// Should match struct dirent.d_name
	if (retVal == true && currDirEntry->d_name)
	{		
		updateThis_ptr->hd_Name = copy_a_string(currDirEntry->d_name);
		
		if (!(updateThis_ptr->hd_Name))
		{
			HARKLE_ERROR(Harkledir, populate_hdEnt_struct, copy_a_string failed);
			retVal = false;
		}
		fprintf(stdout, "hd_Name (%p):\t%s\n", updateThis_ptr->hd_Name, updateThis_ptr->hd_Name);  // DEBUGGING
	}

	//char* hd_AbsName;			// Absolute filename of hd_Name
	if (retVal == true && updateThis_ptr->hd_Name)
	{
		// Check if file or dir
		if (currDirEntry->d_type == DT_DIR)
		{
			isThisAFile = false;
		}
		else
		{
			isThisAFile = true;
		}
		// Build absolute filename
		fprintf(stdout, "populate_hdEnt_struct() calls os_path_join(%s, %s, bool)\n", absPath, updateThis_ptr->hd_Name);  // DEBUGGING
		updateThis_ptr->hd_AbsName = os_path_join(absPath, updateThis_ptr->hd_Name, isThisAFile);

		if (!(updateThis_ptr->hd_AbsName))
		{
			HARKLE_ERROR(Harkledir, populate_hdEnt_struct, os_path_join failed);
			retVal = false;
		}
		fprintf(stdout, "hd_Name (%p):\t%s\n", updateThis_ptr->hd_AbsName, updateThis_ptr->hd_AbsName);  // DEBUGGING
	}

	// ino_t hd_inodeNum;			// Should match struct dirent.d_ino
	if (retVal == true)
	{
		updateThis_ptr->hd_inodeNum = currDirEntry->d_ino;
	}
	
	// unsigned char hd_type; 		// Should match struct dirent.d_type
	if (retVal == true && updateThis_ptr->hd_AbsName)
	{
		// fprintf(stdout, "%s:\t%zd\n", "DT_BLK", (intmax_t)DT_BLK);
		// fprintf(stdout, "%s:\t%zd\n", "DT_CHR", (intmax_t)DT_CHR);
		// fprintf(stdout, "%s:\t%zd\n", "DT_DIR", (intmax_t)DT_DIR);
		// fprintf(stdout, "%s:\t%zd\n", "DT_FIFO", (intmax_t)DT_FIFO);
		// fprintf(stdout, "%s:\t%zd\n", "DT_LNK", (intmax_t)DT_LNK);
		// fprintf(stdout, "%s:\t%zd\n", "DT_REG", (intmax_t)DT_REG);
		// fprintf(stdout, "%s:\t%zd\n", "DT_SOCK", (intmax_t)DT_SOCK);
		// fprintf(stdout, "%s:\t%zd\n", "DT_UNKNOWN", (intmax_t)DT_UNKNOWN);
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
				updateThis_ptr->hd_type = currDirEntry->d_type;
				break;
			default:
				fprintf(stdout, "%s's file type could not be determined.\n", currDirEntry->d_name);
				updateThis_ptr->hd_type = get_a_file_type(updateThis_ptr->hd_AbsName);

				if (updateThis_ptr->hd_type == UCHAR_MAX)
				{
					HARKLE_ERROR(Harkledir, populate_hdEnt_struct, get_a_file_type failed);
					retVal = false;
				}
				break;
		}
		
		// char* hd_symName; 			// If hd_type == DT_LNK, read from readlink()
		if (retVal == true && updateThis_ptr->hd_type == DT_LNK)
		{
			fprintf(stdout, "Calling resolve_symlink(absName == %s (%p), errNum == %d (%p))\n", updateThis_ptr->hd_AbsName, updateThis_ptr->hd_AbsName, errNum, &errNum);  // DEBUGGING
			updateThis_ptr->hd_symName = resolve_symlink(updateThis_ptr->hd_AbsName, &errNum);
			fprintf(stdout, "Called  resolve_symlink(absName == %s (%p), errNum == %d (%p))\n", updateThis_ptr->hd_AbsName, updateThis_ptr->hd_AbsName, errNum, &errNum);  // DEBUGGING

			if (!(updateThis_ptr->hd_symName))
			{
				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, resolve_symlink failed);
				perror(strerror(errNum));  // DEBUGGING
				retVal = false;
			}
		// 	// absPath_ptr = os_path_join()
		// 	// symLinkLength = size_a_file(updateThis_ptr->hd_Name);
		// 	symLinkLength = size_a_file(updateThis_ptr->hd_AbsName, &errNum);
		// 	// fprintf(stdout, "File %s is a symlink of length %jd\n", updateThis_ptr->hd_AbsName, (intmax_t)symLinkLength);  // DEBUGGING
			
		// 	if (symLinkLength == -1)
		// 	{
		// 		switch (errNum)
		// 		{
		// 			case 0:
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with -1 but no errno);
		// 				symLinkLength = PATH_MAX;
		// 				// retVal = false;
		// 				break;
		// 			case EACCES:		// Search permission is denied for one of the directories in the path prefix of path
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with EACCES);
		// 				retVal = false;
		// 				break;
		// 			case EBADF:			// fd is bad
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with EBADF);
		// 				retVal = false;
		// 				break;
		// 			case EFAULT:		// Bad address
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with EFAULT);
		// 				retVal = false;
		// 				break;
		// 			case ELOOP:			// Too many symbolic links encountered while traversing the path
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with ELOOP);
		// 				retVal = false;
		// 				break;
		// 			case ENAMETOOLONG:	// path is too long
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with ENAMETOOLONG);
		// 				retVal = false;
		// 				break;
		// 			case ENOENT:		// A component of path does not exist, or path is an empty string
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with ENOENT);
		// 				retVal = false;
		// 				break;
		// 			case ENOMEM:		// Out of memory (i.e., kernel memory)
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with ENOMEM);
		// 				retVal = false;
		// 				break;
		// 			case ENOTDIR:		// A component of the path prefix of path is not a directory
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with ENOTDIR);
		// 				retVal = false;
		// 				break;
		// 			case EOVERFLOW:		// path or fd refers to a file whose size, inode number, or number of blocks cannot be represented in, respectively, the types off_t, ino_t, or blkcnt_t
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with EOVERFLOW);
		// 				retVal = false;
		// 				break;
		// 			default:
		// 				HARKLE_ERROR(Harkledir, populate_hdEnt_struct, size_a_file failed with unknown ERRNO value);
		// 				retVal = false;
		// 				break;
		// 		}
		// 	}
		// 	else if (symLinkLength == 0)
		// 	{
		// 		// Some symlinks (e.g., /proc, /sys) report 'st_size' as zero.
		// 		// PATH_MAX should be a "good enough" estimate
		// 		symLinkLength = PATH_MAX;
		// 		// fprintf(stdout, "We're guessing a symLinkLength here!\n");  // DEBUGGING
		// 	}
			
		// 	if (retVal == true)
		// 	{
		// 		// Increase the size by one to look for truncation
		// 		symLinkLength++;
				
		// 		// Allocate a buffer
		// 		updateThis_ptr->hd_symName = get_me_a_buffer(symLinkLength);
				
		// 		if (!(updateThis_ptr->hd_symName))
		// 		{
		// 			HARKLE_ERROR(Harkledir, populate_hdEnt_struct, get_me_a_buffer failed);
		// 			retVal = false;
		// 		}
		// 		else
		// 		{
		// 			// Read into that buffer
		// 			numBytesRead = readlink(updateThis_ptr->hd_AbsName, updateThis_ptr->hd_symName, symLinkLength);
		// 			// fprintf(stdout, "readlink(%s, hd_symName, %jd) returned %zd.\n", updateThis_ptr->hd_Name, (intmax_t)symLinkLength, numBytesRead);  // DEBUGGING

		// 			// Validate the read
		// 			if (numBytesRead == -1)
		// 			{
		// 				errNum = errno;

		// 				switch (errNum)
		// 				{
		// 					case 0:
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with -1 but no errno);
		// 						retVal = false;
		// 						break;
		// 					case EACCES:		// Search permission is denied for one of the directories in the path prefix of path
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with EACCES);
		// 						// retVal = false;  // Sometimes you can't access /proc
		// 						if (updateThis_ptr->hd_symName)
		// 						{
		// 							if (false == release_a_string(&(updateThis_ptr->hd_symName)))
		// 							{
		// 								HARKLE_ERROR(Harkledir, populate_hdEnt_struct, release_a_string failed);
		// 								retVal = false;
		// 							}
		// 						}
		// 						break;
		// 					case EFAULT:		// Bad address
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with EFAULT);
		// 						retVal = false;
		// 						break;
		// 					case EINVAL:		// bufsiz is not positive or the named file is not a symbolic link
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with EINVAL);
		// 						retVal = false;
		// 						break;
		// 					case EIO:			// An I/O error occurred while reading from the filesystem
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with EIO);
		// 						retVal = false;
		// 						break;
		// 					case ELOOP:			// Too many symbolic links encountered while traversing the path
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with ELOOP);
		// 						retVal = false;
		// 						break;
		// 					case ENAMETOOLONG:	// path is too long
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with ENAMETOOLONG);
		// 						retVal = false;
		// 						break;
		// 					case ENOENT:		// A component of path does not exist, or path is an empty string
		// 						// HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with ENOENT);
		// 						// retVal = false;  // Weird
		// 						if (updateThis_ptr->hd_symName)
		// 						{
		// 							if (false == release_a_string(&(updateThis_ptr->hd_symName)))
		// 							{
		// 								HARKLE_ERROR(Harkledir, populate_hdEnt_struct, release_a_string failed);
		// 								retVal = false;
		// 							}
		// 						}
		// 						break;
		// 					case ENOMEM:		// Out of memory (i.e., kernel memory)
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with ENOMEM);
		// 						retVal = false;
		// 						break;
		// 					case ENOTDIR:		// A component of the path prefix of path is not a directory
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with ENOTDIR);
		// 						retVal = false;
		// 						break;
		// 					case EBADF:			// fd is bad
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with EBADF);
		// 						retVal = false;
		// 						break;
		// 					default:
		// 						HARKLE_ERROR(Harkledir, populate_hdEnt_struct, readlink failed with unknown ERRNO value);
		// 						retVal = false;
		// 						break;
		// 				}
		// 			}
		// 			else if (numBytesRead == symLinkLength)
		// 			{
		// 				// HARKLE_ERROR(Harkledir, populate_hdEnt_struct, Buffer read may have been truncated);
		// 			}
		// 		}				
		// 	}			
		}
	}

	// CLEAN UP
	// if (absPath_ptr)
	// {
	// 	fprintf(stdout, "Calling release_a_string(absPath_ptr == %s (%p))\n", absPath_ptr, absPath_ptr);  // DEBUGGING
	// 	if (false == release_a_string(&absPath_ptr))
	// 	{
	// 		HARKLE_ERROR(Harkledir, populate_hdEnt_struct, release_a_string has failed);
	// 	}
	// 	fprintf(stdout, "Calling release_a_string(absPath_ptr == %s (%p))\n", absPath_ptr, absPath_ptr);  // DEBUGGING
	// }
	
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
	if (retVal == true && freeThis_ptr)
	{
		// 1. Memset/Free/NULL the struct contents
		if (freeThis_ptr->hd_Name)
		{
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
		}

		if (freeThis_ptr->hd_AbsName)
		{
			// char* hd_AbsName;			// Absolute filename of hd_Name
			temp_ptr = freeThis_ptr->hd_AbsName;
			mrRetVal = release_a_string(&temp_ptr);
			// mrRetVal = release_a_string(&(freeThis_ptr->hd_AbsName));
			
			if (mrRetVal == false)
			{
				HARKLE_ERROR(Harkledir, free_hdEnt_ptr, release_a_string failed);
				retVal = false;	
			}
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


// #define HDIR_DT_BLK			(unsigned int)(1 << 0)
// #define HDIR_DT_CHR			(unsigned int)(1 << 1)
// #define HDIR_DT_DIR			(unsigned int)(1 << 2)
// #define HDIR_DT_FIFO		(unsigned int)(1 << 3)
// #define HDIR_DT_LNK			(unsigned int)(1 << 4)
// #define HDIR_DT_REG			(unsigned int)(1 << 5)
// #define HDIR_DT_SOCK		(unsigned int)(1 << 6)
// #define HDIR_DT_UNKNOWN		(unsigned int)(1 << 7)
// #define HDIR_DT_ALL			(unsigned int)(1 << 8)
/*
	Purpose - Parse directoryDetails arrays into a unique char* array
	Input
		dirStruct_ptr - directoryDetails struct pointer to parse from
		typeFlags - bitwise OR of "typeFlags" MACRO FLAGS
		uniqueEntries
			If true, only unique entries are copied in
			If false, all entries are copied in
		resolveLinks
			If true, hd_symName is used for symbolic link files
			If false, hd_Name is used for all entries
 */
char** parse_dirDetails_to_char_arr(dirDetails_ptr dirStruct_ptr, unsigned int typeFlags, bool uniqueEntries, bool resolveLinks)
{
	// LOCAL VARIABLES
	char** retVal = NULL;
	char** tempRetVal = NULL;  // Copy the strings here first
	hdEnt_ptr* structArray = NULL;  // Used for fileName_arr and dirName_arr as appropriate
	hdEnt_ptr struct_ptr = NULL;  // Used for hdEnt structs in the dirDetails arrays
	bool success = true;  // Make this false if anything fails
	int originalEntries = 0;  // Original size of tempRetVal (for free()ing)
	int totalEntries = 0;  // Size of the tempRetVal
	char* temp_ptr = NULL;  // Use this to dynamically search for hd_Name or hd_symName as appropriate
	int i = 0;  // Iterating variable

	// INPUT VALIDATION
	if (!dirStruct_ptr)
	{
		HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, NULL dirStruct_ptr);
		success = false;
	}
	else if (0 == (typeFlags && HDIR_DT_BLK) && \
			 0 == (typeFlags && HDIR_DT_CHR) && \
			 0 == (typeFlags && HDIR_DT_DIR) && \
			 0 == (typeFlags && HDIR_DT_FIFO) && \
			 0 == (typeFlags && HDIR_DT_LNK) && \
			 0 == (typeFlags && HDIR_DT_REG) && \
			 0 == (typeFlags && HDIR_DT_SOCK) && \
			 0 == (typeFlags && HDIR_DT_UNKNOWN) && \
			 0 == (typeFlags && HDIR_DT_ALL))
	{
		HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, Invalid flags);
		success = false;
	}

	// BEGIN PARSING
	if (success == true)
	{
		// 1. How many total entries?
		// 1.1. Files
		structArray = dirStruct_ptr->fileName_arr;

		if (structArray)
		{
			while (*structArray)
			{
				struct_ptr = *structArray;

				if (true == keep_hdEntry(struct_ptr->hd_type, typeFlags))
				{
					totalEntries++;
				}

				structArray++;
			}
		}

		// 1.2. Directories
		structArray = dirStruct_ptr->dirName_arr;

		if (structArray)
		{
			while (*structArray)
			{
				struct_ptr = *structArray;

				if (true == keep_hdEntry(struct_ptr->hd_type, typeFlags))
				{
					totalEntries++;
				}

				structArray++;
			}
		}

		// 2. Allocate the temp array
		// fprintf(stdout, "About to allocate a buffer of %d bytes\n", totalEntries);  // DEBUGGING
		tempRetVal = get_me_a_buffer_array((size_t)totalEntries, true);

		if (!tempRetVal)
		{
			HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, get_me_a_buffer_array failed);
			success = false;
		}
		else
		{
			// 3. Copy in all the char*s
			// 3.0. Reset the count in case the unique count is less than the total
			originalEntries = totalEntries;
			totalEntries = 0;

			// 3.1. Files
			structArray = dirStruct_ptr->fileName_arr;

			if (structArray)
			{
				while (*structArray)
				{
					struct_ptr = *structArray;

					if (true == keep_hdEntry(struct_ptr->hd_type, typeFlags))
					{
						// Name or Symlink
						if (resolveLinks == true && struct_ptr->hd_symName)
						{
							temp_ptr = struct_ptr->hd_symName;
						}
						else
						{
							temp_ptr = struct_ptr->hd_Name;
						}

						// Does it need to be a unique entry?
						if (uniqueEntries == true)  // Yes, it needs to be unique
						{
							if (-1 == search_char_arr(tempRetVal, temp_ptr))
							{
								// Add it (unique)
								(*(tempRetVal + totalEntries)) = copy_a_string(temp_ptr);

								if (NULL == ((*(tempRetVal + totalEntries))))
								{
									HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, get_me_a_buffer_array failed);
									success = false;
								}
								else
								{
									totalEntries++;
								}
							}
						}
						else  // No, it doesn't need to be unique
						{
							// Add it (anyway)
							(*(tempRetVal + totalEntries)) = copy_a_string(temp_ptr);

							if (NULL == ((*(tempRetVal + totalEntries))))
							{
								HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, get_me_a_buffer_array failed);
								success = false;
							}
							else
							{
								totalEntries++;
							}
						}
					}

					structArray++;
				}
			}

			// 3.2. Directories
			structArray = dirStruct_ptr->dirName_arr;

			if (structArray)
			{
				while (*structArray)
				{
					struct_ptr = *structArray;

					if (true == keep_hdEntry(struct_ptr->hd_type, typeFlags))
					{
						// Name or Symlink
						if (resolveLinks == true && struct_ptr->hd_symName)
						{
							temp_ptr = struct_ptr->hd_symName;
						}
						else
						{
							temp_ptr = struct_ptr->hd_Name;
						}

						// Does it need to be a unique entry?
						if (uniqueEntries == true)  // Yes, it needs to be unique
						{
							if (-1 == search_char_arr(tempRetVal, temp_ptr))
							{
								// Add it (unique)
								(*(tempRetVal + totalEntries)) = copy_a_string(temp_ptr);

								if (NULL == ((*(tempRetVal + totalEntries))))
								{
									HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, copy_a_string failed);
									success = false;
								}
								else
								{
									totalEntries++;
								}
							}
						}
						else  // No, it doesn't need to be unique
						{
							// Add it (anyway)
							(*(tempRetVal + totalEntries)) = copy_a_string(temp_ptr);

							if (NULL == ((*(tempRetVal + totalEntries))))
							{
								HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, copy_a_string failed);
								success = false;
							}
							else
							{
								totalEntries++;
							}
						}
					}

					structArray++;
				}
			}
		}
	}

	// PREPARE THE *REAL* ARRAY
	if (success == true)
	{
		// Allocate the *REAL* array
		retVal = get_me_a_buffer_array(totalEntries * sizeof(char*), true);

		if (!retVal)
		{
			HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, get_me_a_buffer_array failed);
			success = false;
		}
		else
		{
			// Copy all of the char*s into the *REAL* array
			for (i = 0; i < totalEntries; i++)
			{
				(*(retVal + i)) = (*(tempRetVal + i));
				(*(tempRetVal + i)) = NULL;
			}
		}
	}

	// CLEAN UP
	// 1. tempRetVal
	if (tempRetVal)
	{
		if (*tempRetVal && success == false)
		{
			if (false == free_char_arr(&tempRetVal))
			{
				HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, free_char_arr failed on tempRetVal during error condition);
			}
		}
		else
		{
			// 1.1. memset
			if (tempRetVal != memset(tempRetVal, HDIR_MEMSET_DEFAULT, originalEntries * sizeof(char*)))
			{
				HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, memset failed);
			}
			// 1.2. free
			free(tempRetVal);
			// 1.3. NULL
			tempRetVal = NULL;
		}
	}

	if (success == false)
	{
		if (retVal)
		{
			if (false == free_char_arr(&retVal))
			{
				HARKLE_ERROR(Harkledir, parse_dirDetails_to_char_arr, free_char_arr failed during error condition);
			}
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
				retVal = populate_dirDetails_arrays(updateThis_ptr, currDirEntry, updateThis_ptr->dirName);
			}
		} while (currDirEntry && retVal == true);
	}

	// CLEAN UP
	if (cwd)
	{
		if (-1 == closedir(cwd))
		{
			HARKLE_ERROR(Harkledir, populate_dirDetails, closedir failed);
		}
	}

	if (absPath_ptr)
	{
		fprintf(stdout, "Calling release_a_string(absPath_ptr == %s (%p))\n", absPath_ptr, absPath_ptr);  // DEBUGGING
		if (false == release_a_string(&absPath_ptr))
		{
			HARKLE_ERROR(Harkledir, populate_dirDetails, release_a_string failed);
		}		
		fprintf(stdout, "Called  release_a_string(absPath_ptr == %s (%p))\n", absPath_ptr, absPath_ptr);  // DEBUGGING
	}

	// DONE
	return retVal;
}


bool populate_dirDetails_arrays(dirDetails_ptr updateThis_ptr, struct dirent* fileEntry, char* absPath)
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
		HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, updateThis_ptr NULL pointer);
		retVal = false;
	}
	else if (!fileEntry)
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, fileEntry NULL pointer);
		retVal = false;
	}
	else if (!(updateThis_ptr->fileName_arr))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, fileName_arr NULL pointer);
		retVal = false;
	}
	else if (!absPath)
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, absPath NULL pointer);
		retVal = false;
	}
	else if (!(*absPath))
	{
		HARKLE_ERROR(Harkledir, populate_dirDetails_arrays, Empty path);
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
		necessarySize = (*numEntries_ptr) * sizeof(hdEnt_ptr); 			// Total bytes currently stored
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
				(*((*abstractArr_ptr) + (*arraySize_ptr) - 1)) = NULL;
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
			fprintf(stdout, "populate_dirDetails_arrays() calls  populate_hdEnt_struct(%s, %s, %s)\n", "empty struct", fileEntry->d_name, absPath);  // DEBUGGING
			if (true == populate_hdEnt_struct(newStruct, fileEntry, absPath))
			{
				fprintf(stdout, "populate_dirDetails_arrays() called populate_hdEnt_struct(%s, %s, %s)\n", newStruct->hd_AbsName, fileEntry->d_name, absPath);  // DEBUGGING
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


bool keep_hdEntry(unsigned char hdEntryType, unsigned int typeFlags)
{
	// LOCAL VARIABLES
	bool retVal = false;

	// INPUT VALIDATION
	if ((hdEntryType != DT_BLK) && \
		(hdEntryType != DT_CHR) && \
		(hdEntryType != DT_DIR) && \
		(hdEntryType != DT_FIFO) && \
		(hdEntryType != DT_LNK) && \
		(hdEntryType != DT_REG) && \
		(hdEntryType != DT_SOCK) && \
		(hdEntryType != DT_UNKNOWN))
	{
		HARKLE_ERROR(Harkledir, keep_hdEntry, hdEntryType);
	}
	else if (0 == (typeFlags & HDIR_DT_BLK) && \
			 0 == (typeFlags & HDIR_DT_CHR) && \
			 0 == (typeFlags & HDIR_DT_DIR) && \
			 0 == (typeFlags & HDIR_DT_FIFO) && \
			 0 == (typeFlags & HDIR_DT_LNK) && \
			 0 == (typeFlags & HDIR_DT_REG) && \
			 0 == (typeFlags & HDIR_DT_SOCK) && \
			 0 == (typeFlags & HDIR_DT_UNKNOWN) && \
			 0 == (typeFlags & HDIR_DT_ALL))
	{
		HARKLE_ERROR(Harkledir, keep_hdEntry, Invalid typeFlags);
	}

	// DECIDE
	switch (hdEntryType)
	{
		case DT_BLK:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_BLK) 	{ retVal = true; }
			break;
		case DT_CHR:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_CHR) 	{ retVal = true; }
			break;
		case DT_DIR:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_DIR) 	{ retVal = true; }
			break;
		case DT_FIFO:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_FIFO) 	{ retVal = true; }
			break;
		case DT_LNK:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_LNK) 	{ retVal = true; }
			break;
		case DT_REG:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_REG) 	{ retVal = true; }
			break;
		case DT_SOCK:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_SOCK) 	{ retVal = true; }
			break;
		case DT_UNKNOWN:
			if (typeFlags & HDIR_DT_ALL || typeFlags & HDIR_DT_UNKNOWN)	{ retVal = true; }
			break;
		default:
			HARKLE_ERROR(Harkledir, keep_hdEntry, Invalid typeFlags?!  How did we get here?!);
			break;
	}

	// DONE
	return retVal;
}

	
//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


char* resolve_symlink(char* absSymPathName, int* errNum)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char buff[HDIR_BIG_BUFF_SIZE + 1] = { 0 };
	int buffStrLen = 0;  // Length of the string read into buff
	ssize_t numBytesRead = 0;  // Return value from readlink()
	bool success = true;  // Make this false if anything fails
	puts("HERE");
	// INPUT VALIDATION
	if (!absSymPathName)
	{
		HARKLE_ERROR(Harkledir, resolve_symlink, absSymPathName NULL pointer);
		success = false;
	}
	else if (!(*absSymPathName))
	{
		HARKLE_ERROR(Harkledir, resolve_symlink, absSymPathName empty string);
		success = false;
	}
	else if (!errNum)
	{
		HARKLE_ERROR(Harkledir, resolve_symlink, errNum NULL pointer);
		success = false;
	}
	else
	{
		*errNum = 0;	
	}
	
	// RESOLVE SYMLINK
	// 0. Verify it's a symlink?
	if (success == true)
	{	
		// Implement later?
	}
	
	// 1. readlink()
	if (success == true)
	{	
		numBytesRead = readlink(absSymPathName, buff, HDIR_BIG_BUFF_SIZE);
		
		if (numBytesRead == -1)
		{
			*errNum = errno;
			HARKLE_ERROR(Harkledir, resolve_symlink, readlink failed);
			fprintf(stderr, "Failed to open:\t%s\n", absSymPathName);  // DEBUGGING
			perror(strerror(*errNum));  // DEBUGGING
			success = false;
		}
	}
	
	// 2. Size buff[]
	if (success == true)
	{	
		buffStrLen = strlen(buff);
		
		if (numBytesRead != buffStrLen)
		{
			HARKLE_ERROR(Harkledir, resolve_symlink, readlink and strlen mismatch);
			success = false;	
		}
	}
	
	// 3. Allocate and copy
	if (success == true)
	{	
		retVal = copy_a_string(buff);
		
		if (!retVal)
		{
			HARKLE_ERROR(Harkledir, resolve_symlink, copy_a_string failed);
			success = false;
		}
	}
	
	// CLEAN UP
	if (success == false)
	{	
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Harkledir, resolve_symlink, release_a_string failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


