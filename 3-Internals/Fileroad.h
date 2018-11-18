/*
	This header will be a collection of all my nit-noid file I/O
		functions I've ever written.  Frankly, I'm sick of
		rewriting them every time.
 */

#ifndef __FILEROAD__
#define __FILEROAD__

#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// FILE*
#include <sys/types.h>	// off_t

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// INPUT FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose: Remove any pesky characters lurking around on the stdin stream
	Input - None
	Output - None
	Notes:
		Will read from stdin until an EOF or newline is read
 */
void clear_stdin(void);


/*
	Purpose: Read a number from stdin
	Input - None
	Output - A signed integer
	Notes:
		Calls clear_stdin() after reading to clear the stream
 */
int read_a_num(void);


/*
	Purpose: Read a number from stdin into a heap-allocated char array
	Input - None
	Output
		On success, a heap-allocated char array containing the number read
		On failure, NULL
	Notes:
		Will return NULL if any input is non-character
		Will return NULL if input exceeds buffer length
        It is the caller's responsibility to free the char* returned by 
        	this function

 */
char* buff_a_num(void);


/*
	Purpose: Split a string by a given character
	Input
		haystack - nul-terminated char array
		splitChar - character to 'split' the haystack on
	Output - Heap-allocated, NULL terminated, array of heap-allocated char 
		pointers to each section 'split' string found in haystack
	Notes:
		haystack should not be modified
		The return value will not provide pointers into haystack
		The caller is responsible for free()ing all of the char pointers
			in the array in addition to the array itself.
		Successive occurrences of splitChar will be treated as one
 */
char** split_lines(char* haystack, char splitChar);


/*
	Purpose: Find a needle_ptr in a haystack_arr
	Input
		haystack_arr - A NULL-terminated array of char*s
		needle_ptr - A nul-terminated char*
	Output
		haystack_arr index if found
		-1 otherwise
	Notes:
		This function utilizes strcmp()
 */
int search_char_arr(char** haystack_arr, char* needle_ptr);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Open a file, read the contents into a buffer, close the file
    Input
        fileName - nul-terminated char array of the file to read
    Ouput - Heap-allocated array containing the contents of fileName
            on success, NULL on failure
    Notes:
        The caller is responsible for free()ing the return value
        Utilizes fopen()
 */
char* fread_a_file(char* fileName);


/*
    Purpose - Open a fd, read the contents into a buffer, close the fd
    Input
        fileName - nul-terminated char array of the file to read
    Ouput - Heap-allocated array containing the contents of fileName
            on success, NULL on failure
    Notes:
        The caller is responsible for free()ing the return value
        Utilizes read()
 */
char* read_a_file(char* fileName);


/*
	Purpose - Utilize stat to size a file
	Input
        fileName - nul-terminated char array of the file to read
        errNum - [OUT] Memory location to store errno value
	Output
		On success, total size, of fileName, in bytes
		On failure, returns -1 and stores errno in errNum
	Notes:
		This function calls stat()	
 */
off_t size_a_file(char* fileName, int* errNum);


/*
	Purpose - Utilize stat to size a file
	Input
        fileDesc - File descriptor for an open file
        errNum - [OUT] Memory location to store errno value
	Output
		On success, total size, of file descriptor, in bytes
		On failure, returns -1 and stores errno in errNum
	Notes:
		This function calls fstat()
 */
off_t size_a_file_desc(int fileDesc, int* errNum);


/*
	Purpose - Determine the size of a file byte by byte
	Input
		openFile - FILE* to an already opened file
	Output
		On success, total size in bytes
		On failure, returns -1
	Notes:
		Rewinds the file prior to read
		Utilizes fgetc() to determine the file size
		Rewinds the file after the read
 */
size_t size_a_file_ptr(FILE* openFile);


/*
	Purpose - Utilize stat to determine a file's type
	Input
        fileName - nul-terminated char array of the file to read
	Output
		On success, file's type
		On failure, UCHAR_MAX
	Notes:
		This should be called during those pesky times a dirent struct
			doesn't have your answer
 */
unsigned char get_a_file_type(char* fileName);


/*
	Purpose - Quick, safe, "handled" way of determining if a path is a file
	Input
		path_ptr - nul-terminated string presumably representing a path
	Output - True if path_ptr is a file that exists, false otherwise
	Notes:
		This function should not raise any errors
		This function will take care to 'zeroize' errno before returning
		This function calls os_path_exists()
		Currently, only regular files (DT_REG) are treated as "files"
		Symbolic links (DT_LNK) are not treated as "files"
 */
bool os_path_isfile(char* path_ptr);


/*
	Purpose - Quick, safe, "handled" way of determining if a path or file exists
	Input
		path_ptr - nul-terminated string presumably representing a path
	Ouput - True if file exists, false otherwise
	Notes:
		This function should not raise any errors
		This function will take care to 'zeroize' errno before returning
 */
bool os_path_exists(char* path_ptr);


/*
	Purpose - Replicate os.path.join (in a very hacky way)
	Input
		path_ptr - nul-terminated string presumably representing a path
		join_ptr - nul-terminate string presumably representing something to add
			to path_ptr
		isFile - true if join_ptr is a file, false if join_ptr is a directory
	Output
		On success, a heap-allocated, nul-terminated string containing
			(essentially) a strcat of path_ptr and join_ptr
		On failure, NULL
	Notes:
		It is the caller's responsibility to free the return value
	Examples:
		path_ptr == "/proc", join_ptr == "31337", isFile == false, returns "/proc/31337/"
		path_ptr == "/proc", join_ptr == "31337", isFile == true, returns "/proc/31337"
		path_ptr == "/proc/", join_ptr == "/31337/", isFile == false, returns "/proc/31337/"
		path_ptr == "/proc/", join_ptr == "/31337/", isFile == true, returns "/proc/31337"
 */
char* os_path_join(char* path_ptr, char* join_ptr, bool isFile);


/*
	Purpose - Extract the base filename from a relative or absolute filename
	Input
		path_ptr - Nul-terminated relative or absolute filename to parse
	Output
		On success, heap-allocated string containing the base filename
		On failure, NULL
	Notes:
		This function is essentially a wrapper around libgen.h's basename()
		This function heap-allocates responses instead of dealing with shady
			"somewhere" pointers that may or may not need to be free()d in some
			fashion or another
		It is the caller's responsibility to free() the return value of this function
 */
char* os_path_basename(char* path_ptr);


/*
	Purpose - Extract the directory path from a relative or absolute filename
	Input
		path_ptr - Nul-terminated relative or absolute filename to parse
	Output
		On success, heap-allocated string containing the directory path
		On failure, NULL
	Notes:
		This function is essentially a wrapper around libgen.h's dirname()
		This function heap-allocates responses instead of dealing with shady
			"somewhere" pointers that may or may not need to be free()d in some
			fashion or another
		It is the caller's responsibility to free() the return value of this function
 */
char* os_path_dirname(char* path_ptr);


/*
	Purpose - Rewind a file descriptor
	Input
		fileDesc - The file descriptor
		errNum - A pointer to place the value of errno in on error
	Output
		On success, returns true
		On failure, returns false and places errno in errNum
	Notes:
		A simple function, to be certain, but I don't want to write it more
			than once.
 */
bool rewind_a_file_desc(int fileDesc, int* errNum);


/*
	Purpose - Clean a potential filename
	Input
		dirtyFile - C string representing a potential filename
		inPlace - If true, modified in place.  Otherwise, a copy
			is returned.
	Output
		On success:
			inPlace == true, returns dirtyFile
			inPlace == false, heap-allocated string with the modified filename
		On failure, NULL
	Notes:
		It is the caller's responsiblity to free() the return value if
			inPlace was true
 */
char* clean_filename(char* dirtyFile, bool inPlace);


/*
	Purpose - Resolve a symbolic link into a dynamically allocated buffer
	Input
		symlinkName - C string representing a symbolic link
	Output
		On success - Symbolic link resolution stored in a dynamically sized
			buffer allocated in heap memory
		On failure, NULL
	Notes:
		Will continue to allocate memory until buffer size large enough
		Does *not* rely on PATH_MAX
 */
char *resolve_symlink(char *symlinkName);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __FILEROAD__
