#include <stdlib.h>		// calloc
#include "Harkledir.h"
#include <stdbool.h>	// bool, true, false


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
bool free_dirDetails_ptr(dirDetails_ptr* oldStruct_ptr);
