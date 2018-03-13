#ifndef __FILEROAD_DESCRIPTORS__
#define __FILEROAD_DESCRIPTORS__

// #include <inttypes.h>       // uintmax_t
#include <stdbool.h>	    // bool, true, false
#include <sys/stat.h>		// mode_t

#ifndef NULL
#define NULL ((void*)0)
#endif  // NULL

typedef struct fileDescriptorDetails
{
	char* filename_ptr;     // Path to file
	int fileDesc;           // File descriptor
	off_t fileSize;     	// Actual size of file
	int fileDescFlags;		// File descriptor flags
	int fileStatFlags;		// File status flags
} fdDetails, *fdDetails_ptr;

typedef struct redirectBinOutput
{
	char* binName;			// Just the binary name
	char* binPath;  		// NOT IMPLEMENTED... Path to the binary
	char** fullCmd;			// argv[1]... do NOT free()!
	char* outputFile;		// File capturing binary's stdout
	char* errorsFile;		// File capturing binary's stderr
	int readPipe;			// Implement later... Binary (child) reads stdin from here
	int writePipe;			// Implement later... redirect_bin_output.exe (parent) writes binary's input here
} rBinDat, *rBinDat_ptr;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// rBinDat FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Dynamically allocate a struct pointer
	Input - None
	Output - Pointer to a redirectBinOutput struct on the heap
	Notes:
		rBinDat_ptr must be free()'d by the calling function
 */
rBinDat_ptr create_rBinDat_ptr(void);


/*
	Purpose - Populate a redirectBinOutput struct
	Input
		binaryName - Relative or absolute path string to the binary being redirected
		binArgs - A copy of main()'s argv starting at index 1
	Output - Pointer to a redirectBinOutput struct on the heap whose binName is
		populated
	Notes:
		Calls create_rBinDat_ptr() to allocate the struct
		Caller is responsible for free()ing all char pointer members and the 
			struct itself
 */
rBinDat_ptr build_rBinDat_ptr(char* binaryName, char** binArgs);


/*
	Purpose - Free a redirectBinOutput struct on the heap
	Input - Pointer to a redirectBinOutput pointer to free
	Output - None
	Notes:
		Will memset(0x0), free, and NULL all char pointer members
		Will zeroize all other members
 */
bool free_rBinDat_ptr(rBinDat_ptr* oldStruct_ptr);


/*
	Purpose
		The details inside the binToWrap struct pointer should all point to
			a binary file (and arguments, if necessary) in order to redirect
			it's output.  The stdout of binName will be redirected to
			outputFile and the stderr will be redirected to errorsFile.
			The binary itself will be executed with all of the parameters
			found in fullCmd.
	Input
		binToWrap - Pointer to a redirectBinOutput struct with details about
			the binary to wrap
	Output
		_exit() code of the child process fork()ed
		-1 if an error is encountered during fork()ing
		-2 if an error is encountered prior to fork()ing
	Notes:
		This function call fork() and execvp()
 */
int wrap_bin(rBinDat_ptr binToWrap);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// rBinDat FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// fdDetails FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Dynamically allocate a struct pointer
	Input - None
	Output - Pointer to a fileDescriptorDetails struct on the heap
	Notes:
		fdDetails_ptr must be free()'d by the calling function
 */
fdDetails_ptr create_fdDetails_ptr(void);


/*
	Purpose - Open a file and populate a fileDescriptorDetails struct
	Input
		fname - Filename to open
		flags
			Access modes: O_RDONLY, O_WRONLY, or O_RDWR
			File creation flags: O_CLOEXEC, O_CREAT, O_DIRECTORY, O_EXCL, 
			                     O_NOCTTY, O_NOFOLLOW, O_TMPFILE, and O_TRUNC
		mode - A bitwise OR of file creation flags and file status flags
	Output - Pointer to a fileDescriptorDetails struct
	Notes:
		fdDetails_ptr must be free()'d by the calling function
		fdDetails_ptr->filename_ptr must be free()'d by the calling function
        This function calls create_fdDetails_ptr() to allocate a struct
 */
fdDetails_ptr open_fd(const char* fname, int flags, mode_t mode);


/*
	Purpose - Free a fileDescriptorDetails struct on the heap
	Input - Pointer to a fileDescriptorDetails pointer to free
	Output - None
	Notes:
		Will memset(0x0), free, and NULL filename_ptr
		Will zeroize all other members
		Closing the file descriptor is the responsibility of the
            calling function
 */
void free_fdDetails(fdDetails_ptr* oldStruct_ptr);


/*
	Purpose - Validate the contents of a fileDescriptorDetails struct pointer
	Input - Pointer to a fileDescriptorDetails struct
	Output - true if valid, otherwise false
	Notes:
		Can not validate memType against the type of memory allocated
		Merely checks that memType is valid
 */
bool validate_fdDetails(fdDetails_ptr checkThis_ptr);


/*
    Purpose - Update the details of a fileDescriptorDetails struct pointer
    Input - Pointer to a fileDescriptorDetails struct
    Output - 0 on success, error code on failure
 */
int update_fdDetails(fdDetails_ptr updateThis_ptr);


/*
	Purpose - Set an I/O Operating Mode flag(s) for a file descriptor
	Input
		updateThis_ptr - fileDescriptorDetails struct pointer
		setThisFlag - Operating mode flag(s) to set for updateThis_ptr's fd
	Output - true on success, false on failure
	Notes:
		This function will preserve the unchanged flags
		This function calls _____()
 */
bool set_oper_mode_flag(fdDetails_ptr updateThis_ptr, int setThisFlag);


/*
	Purpose - Clear an I/O Operating Mode flag(s) for a file descriptor
	Input
		updateThis_ptr - fileDescriptorDetails struct pointer
		clrThisFlag - Operating mode flag(s) to set for updateThis_ptr's fd
	Output - true on success, false on failure
	Notes:
		This function will preserve the unchanged flags
		This function calls write_oper_mode_flags()
 */
bool clear_oper_mode_flag(fdDetails_ptr updateThis_ptr, int clrThisFlag);


/*
	Purpose - Overwrite all I/O Operating Mode flags for a file descriptor
	Input
		updateThis_ptr - fileDescriptorDetails struct pointer
		setTheseFlags - Operating mode flags to set for updateThis_ptr's fd
	Output - true on success, false on failure
 */
bool write_oper_mode_flags(fdDetails_ptr updateThis_ptr, int setTheseFlags);


//////////////////////////////////////////////////////////////////////////////
////////////////////////// fdDetails FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Determine the file size of a file descriptor
	Input - An open file descriptor
	Output - Size of the file descriptor
	Notes:
		This function will rewind() the file descriptor
 */
long get_file_len(int fileDesc);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// GENERAL FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __FILEROAD_DESCRIPTORS__
