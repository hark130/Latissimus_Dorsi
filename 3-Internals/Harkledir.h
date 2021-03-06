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

/* parse_dirDetails_to_char_arr() "typeFlags" MACRO FLAGS */
#define HDIR_DT_BLK			(unsigned int)(1 << 0)
#define HDIR_DT_CHR			(unsigned int)(1 << 1)
#define HDIR_DT_DIR			(unsigned int)(1 << 2)
#define HDIR_DT_FIFO		(unsigned int)(1 << 3)
#define HDIR_DT_LNK			(unsigned int)(1 << 4)
#define HDIR_DT_REG			(unsigned int)(1 << 5)
#define HDIR_DT_SOCK		(unsigned int)(1 << 6)
#define HDIR_DT_UNKNOWN		(unsigned int)(1 << 7)
#define HDIR_DT_ALL			(unsigned int)(1 << 8)

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
char** parse_dirDetails_to_char_arr(dirDetails_ptr dirStruct_ptr, unsigned int typeFlags, bool uniqueEntries, bool resolveLinks);

//////////////////////////////////////////////////////////////////////////////
////////////////////// DIRECTORYDETAILS FUNCTIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Resolve symbolic links into heap-allocated, minimally-sized, char arrays
	Input
		absSymPathName - An absolute path to the filename of a symbolic link
		errNum - A pointer to an integer variable to store errno on errors
	Output
		On success, a heap-allocated, nul-terminated char array holding the resolved symlink
		On failure, returns NULL, errNum will be populated with the errno value
	Notes:
		Essentially, this is a wrapper around readlink
		It is the caller's responsibility to free the returned pointer
 */
char* resolve_symlink(char* absSymPathName, int* errNum);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __HARKLEDIR__
