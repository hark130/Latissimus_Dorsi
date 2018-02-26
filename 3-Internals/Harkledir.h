/*
	VERSION 1 - Initial implementation
	VERSION 2
		Adding an harkleDirEnt struct
		Changing dirDetails to hold arrays of harkleDirEnt struct pointers, as appropriate
		Changing all of the directoryDetails functions/functionality to reflect this change
		Adding similar create, populate, and free functions/functionality to support the new inodeDetails struct
 */

#ifndef __HARKLEDIR__
#define __HARKLEDIR__

#include <stdbool.h>	// bool, true, false
#include <stdlib.h>		// size_t

typdef struct harkleDirEnt
{
	char* hd_Name;				// Should match struct dirent.d_name
	ino_t hd_inodeNum;			// Should match struct dirent.d_ino
	unsigned char hd_type; 		// Should match struct dirent.d_type
	char* hd_symName;			// If hd_type == DT_LNK, read from readlink()
} hdEnt, *hdEnt_ptr;

typedef struct directoryDetails
{
	char* dirName;				// Directory name to walk
	int numFiles;				// Number of hdEnt struct pointers in fileName_arr
	hdEnt_ptr* fileName_arr;	// Array of pointers to hdEnt structs storing file information
	size_t fileArrSize;			// Allocated bytes for fileName_arr
	int numDirs;				// Number of hdEnt struct pointers in dirName_arr
	hdEnt_ptr* dirName_arr;		// Array of pointers to hdEnt structs storing directory information
	size_t dirArrSize;			// Allocated bytes for dirName_arr
} dirDetails, *dirDetails_ptr;


/*
	Purpose - Dynamically allocate a struct pointer on the heap
	Input - None
	Output - Pointer to a directoryDetails struct on the heap
	Notes:
		dirDetails_ptr must be free()'d by the calling function
 */
dirDetails_ptr create_dirDetails_ptr(void);


/*
	Purpose - Read and parse a given directory name
	Input
		directoryName - stack-allocated, nul-terminated, directory name
	Output - heap-allocated, fully populated, directoryDetails struct pointer
	Notes:
		If directoryName is NULL or empty, will default to cwd
 */
dirDetails_ptr open_dir(char* directoryName);


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


#endif  // __HARKLEDIR__
