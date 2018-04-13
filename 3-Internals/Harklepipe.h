#ifndef __HARKLEPIPE__
#define __HARKLEPIPE__

#include <stdlib.h>			// size_t

// MACROs to help properly access int array indices
#define HPIPE_READ 0
#define HPIPE_WRITE 1


/*
	PURPOSE - Make plumbing easy
	INPUT
		emptyPipes - Integer array of dimension 2 to store the resulting file descriptors
		flags - Flags to pass to pipe2() if appropriate
	OUTPUT
		On success, 0
		On failure, errno
	NOTES
		Based on MACROS and the value of flags, make_a_pipe() will call pipe2() instead
			of pipe() in the right circumstances
		This function will return the original values of emptyPipes to their original
			indices in the event of failure... just in case pipe()/pipe2() changed
			something.
 */
int make_a_pipe(int emptyPipes[2], int flags);


/*
	PURPOSE - Read data from a pipe's file descriptor stopping at the 'stop' char
	INPUT
		readFD - The pipe's read file descriptor
		stop - A character that, if read, will cause this function
			to stop reading and return with everything read
			up until that point
		errNumber - [OUT] A pointer to a location to store errno upon error
	OUTPUT
		On sucess, heap-allocated copy of what was read
		On failure, NULL (errNumber is updated with errno)
	NOTES
		This function will not close the file descriptor
		It is the caller's responsibility to free the memory returned
		It is the caller's responsibility to sychronize read and write
			operations with whatever is on the write end of this pipe
		This function will stop reading when it encounters a newline or a nul
			character, regardless of what value 'stop' is
		Maybe later we'll support some realloc functionality to
			continue reading.  Right now, I don't need it and I
			just want to get the bytes flowing through these pipes.
 */
char* read_a_pipe(int readFD, char stop, int* errNumber);


/*
	PURPOSE - Write "numBytes" worth of "writeStr" into the writeFD file
		descriptor
	INPUT
		writeFD - The pipe's write file descriptor
		writeStr - Buffer to write to writeFD
		numBytpes - Amount of data to copy from writeStr into writeFD
	OUTPUT
		On success, 0
		On failure, error code
	NOTES
		This function will not close the file descriptor
 */
int write_a_pipe(int writeFD, void* writeStr, size_t numBytes);

#endif  // __HARKLEPIPE__