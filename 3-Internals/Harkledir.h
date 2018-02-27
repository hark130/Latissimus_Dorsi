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

#include <dirent.h>		// struct dirent
#include <stdbool.h>	// bool, true, false
#include <stdlib.h>		// size_t
#include <sys/types.h>	// ino_t

typedef struct harkleDirEnt
{
	char* hd_Name;				// Should match struct dirent.d_name
	char* hd_AbsName;			// Absolute filename of hd_Name
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////// HARKLEDIRENT FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Dynamically allocate a harkleDirEnt struct pointer on the heap
	Input - None
	Output - Pointer to a harkleDirEnt struct on the heap
	Notes:
		hdEnt_ptr must be free()'d by the calling function (see: free_hdEnt_ptr())
 */
hdEnt_ptr create_hdEnt_ptr(void);


/*
	Purpose - Populate a harkleDirEnt struct pointer with details from dirent (and other) sources
	Input
		updateThis_ptr - [OUT] harkleDirEnt struct pointer to populate
		currDirEntry - dirent struct pointer to gather details from
		absPath - nul-terminated absolute path to the file at *currDirEntry
	Output - true on success, false on failure
	Notes:
		Will call readlink() to resolve any symbolic link "type"s into hd_symName
		Will likely make multiple calls to Memoroad's copy_a_string()
 */
bool populate_hdEnt_struct(hdEnt_ptr updateThis_ptr, struct dirent* currDirEntry, char* absPath);


/*
	Purpose - Zeroize, nullify, and free a heap-allocated harkleDirEnt struct pointer
	Input
		oldStruct_ptr - Pointer to a hdEnt_ptr
	Output - true on success, false on failure
	Notes:
		Will likely make multiple calls to Memoroad's release_a_string()
 */
bool free_hdEnt_ptr(hdEnt_ptr* oldStruct_ptr);


//////////////////////////////////////////////////////////////////////////////
//////////////////////// HARKLEDIRENT FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////// DIRECTORYDETAILS FUNCTIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Dynamically allocate a directoryDetails struct pointer on the heap
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


//////////////////////////////////////////////////////////////////////////////
////////////////////// DIRECTORYDETAILS FUNCTIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __HARKLEDIR__
