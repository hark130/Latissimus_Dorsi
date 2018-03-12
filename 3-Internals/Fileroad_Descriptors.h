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
/*
	NOTE: Updates to the fileDetails struct 
		constitutes a change to the following functions:
	- free_fdDetails()
	- open_fd()
	- validate_fdDetails()
	- update_fdDetails()
 */


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


/*
	Purpose - Determine the file size of a file descriptor
	Input - An open file descriptor
	Output - Size of the file descriptor
	Notes:
		This function will rewind() the file descriptor
 */
long get_file_len(int fileDesc);


#endif  // __FILEROAD_DESCRIPTORS__