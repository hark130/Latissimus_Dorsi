/*
	This header will be a collection of all my nit-noid file I/O
		functions I've ever written.  Frankly, I'm sick of
		rewriting them every time.
 */

#ifndef __FILEROAD__
#define __FILEROAD__

#include <stdbool.h>	// bool, true, false
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


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
    Purpose - Open a fd, read the contents into a buffer, close the fd
    Input
        fileName - nul-terminated char array of the file to read
    Ouput - Heap-allocated array containing the contents of fileName
            on success, NULL on failure
    Notes:
        The caller is responsible for free()ing the return value
 */
char* read_a_file(char* fileName);


/*
	Purpose - Utilize stat to size a file
	Input
        fileName - nul-terminated char array of the file to read
	Output
		On success, total size, of fileName, in bytes
		On failure, -1
	Notes:
		This function calls stat()
		The return value is converted from data type off_t	
 */
off_t size_a_file(char* fileName);


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


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __FILEROAD__
