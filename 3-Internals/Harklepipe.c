#include <errno.h>			// errno
#include "Fileroad_Descriptors.h"	// set_fd_flags()
#include "Harklepipe.h"		// HPIPE_READ, HPIPE_WRITE
#include "Harklerror.h"		// HARKLE_ERROR
#include "Memoroad.h"		
#include <stdbool.h>		// bool, true, false
#include <stdio.h>			// fprintf()
#include <string.h>			// strerror()
#include <unistd.h>			// pipe(), pipe2(), read()

#define HP_BUFF_SIZE 64


int make_a_pipe(int emptyPipes[2], int flags)
{
	// LOCAL VARIABLES
	int errNum = 0;  // Will hold errno on failure
	int retVal = 0;
	bool success = true;  // If anything fails, make this false
	int readFD = 0;  // emptyPipes[HPIPE_READ]
	int writeFD = 0;  // emptyPipes[HPIPE_WRITE]

	// INPUT VALIDATION
	if (!emptyPipes)
	{
		HARKLE_ERROR(Harklepipe, make_a_pipe, NULL pointer);
		success = false;
	}
	else
	{
		readFD = emptyPipes[HPIPE_READ];
		writeFD = emptyPipes[HPIPE_WRITE];
	}

	// SYSTEM CALL
	if (true == success)
	{
		// Conditional call
		if (flags)
		{
			#if defined _GNU_SOURCE && defined __USE_GNU
			retVal = pipe2(emptyPipes, flags);
			#else
			retVal = pipe(emptyPipes);
			#endif  // _GNU_SOURCE && __USE_GNU
		}
		else
		{
			retVal = pipe(emptyPipes);
		}

		// Respond to return value
		if (retVal)
		{
			errNum = errno;
			success = false;

			if (flags)
			{
				#ifdef _GNU_SOURCE
				HARKLE_ERROR(Harklepipe, make_a_pipe, pipe2 failed);
				#else
				HARKLE_ERROR(Harklepipe, make_a_pipe, pipe failed);
				#endif  // _GNU_SOURCE
			}
			else
			{
				HARKLE_ERROR(Harklepipe, make_a_pipe, pipe failed);
			}
			fprintf(stderr, "Function call returned errno:\t%s\n", strerror(errNum));
		}
		else
		{
			if (flags)
			{
				#ifndef _GNU_SOURCE
				errNum = set_fd_flags(emptyPipes[HPIPE_READ], flags, true);

				if (errNum)
				{
					HARKLE_ERROR(Harklepipe, make_a_pipe, set_fd_flags failed);
					fprintf(stderr, "set_fd_flags() return errno:\t%s\n", strerror(errNum));
					success = false;
				}
				else
				{
					errNum = set_fd_flags(emptyPipes[HPIPE_WRITE], flags, true);

					if (errNum)
					{
						HARKLE_ERROR(Harklepipe, make_a_pipe, set_fd_flags failed);
						fprintf(stderr, "set_fd_flags() return errno:\t%s\n", strerror(errNum));
						success = false;
					}
				}
				#endif  // _GNU_SOURCE
			}
		}
	}

	// CLEAN UP
	if (false == success)
	{
		// Read fd
		if (readFD != emptyPipes[HPIPE_READ])
		{
			close(emptyPipes[HPIPE_READ]);
			emptyPipes[HPIPE_READ] = readFD;
		}
		// Write fd
		if (writeFD != emptyPipes[HPIPE_WRITE])
		{
			close(emptyPipes[HPIPE_WRITE]);
			emptyPipes[HPIPE_WRITE] = writeFD;
		}
	}

	// DONE
	return retVal;
}


char* read_a_pipe(int readFD, char stop, int* errNumber)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	char localBuff[HP_BUFF_SIZE + 1] = { 0 };  // Local buffer to read into
	char* tmp_ptr = localBuff;  // Iterate over localBuff
	int readCnt = 0;  // Number of characters read
	bool success = true;  // Make this false if anything fails
	ssize_t readRetVal = 0;  // Return value from read function call
	int errNum = 0;  // Here to capture errno on errors
	size_t numBytes = 1;  // Number of bytes to read at one time

	// INPUT VALIDATION
	if (readFD < 0)
	{
		HARKLE_ERROR(Harklepipe, read_a_pipe, Invalid file descriptor);
		success = false;
	}
	else if (!errNumber)
	{
		HARKLE_ERROR(Harklepipe, read_a_pipe, NULL pointer);
		success = false;
	}
	else
	{
		*errNumber = 0;  // Initialize errNumber
	}

	// BEGIN READING
	while (true == success && readCnt < HP_BUFF_SIZE)
	{
		readRetVal = read(readFD, tmp_ptr, numBytes);

		if (-1 == readRetVal)
		{
			errNum = errno;
			*errNumber = errno;
			// HARKLE_ERROR(Harklepipe, read_a_pipe, read failed);
			// fprintf(stderr, "read() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else if (numBytes != readRetVal)
		{
			HARKLE_ERROR(Harklepipe, read_a_pipe, read mismatch);
			success = false;
		}
		else
		{
			readCnt++;  // One character was read
			// fprintf(stdout, "Just read a %c (%d)\n", *tmp_ptr, *tmp_ptr);  // DEBUGGING

			if (0 == *tmp_ptr || '\n' == *tmp_ptr || stop == *tmp_ptr)
			{
				// Done reading
				break;
			}
			else
			{
				tmp_ptr++;  // Advance to the next index
			}
		}
	}

	// COPY DATA
	if (true == success)
	{
		// fprintf(stdout, "Done reading!\n");  // DEBUGGING
		retVal = copy_a_string(localBuff);

		if (!retVal)
		{
			HARKLE_ERROR(Harklepipe, read_a_pipe, copy_a_string failed);
			success = false;
		}
	}

	// CLEAN UP
	// localBuff
	if (*localBuff)
	{
		if (localBuff != memset(localBuff, 0x0, readCnt))
		{
			HARKLE_ERROR(Harklepipe, read_a_pipe, memset failed);
		}
	}

	if (false == success)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(Harklepipe, read_a_pipe, release_a_string failed);
			}
		}
	}

	// DONE
	return retVal;
}


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
int write_a_pipe(int writeFD, void* writeStr, size_t numBytes)
{
	// LOCAL VARIABLES
	int retVal = 0;
	int errNum = 0;  // Here to capture errno if anything fails
	bool success = true;  // Set this to false if anything fails
	ssize_t writeRetVal = 0;  // Return value from read function call

	// INPUT VALIDATION
	if (writeFD < 0)
	{
		HARKLE_ERROR(Harklepipe, write_a_pipe, Invalid file descriptor);
		success = false;
	}
	else if (!writeStr)
	{
		HARKLE_ERROR(Harklepipe, write_a_pipe, NULL pointer);
		success = false;
	}
	else if (numBytes <= 0)
	{
		HARKLE_ERROR(Harklepipe, write_a_pipe, Invalid number of bytes);
		success = false;
	}

	// WRITE
	if (true == success)
	{
		writeRetVal = write(writeFD, writeStr, numBytes);

		if (-1 == writeRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(Harklepipe, read_a_pipe, write failed);
			fprintf(stderr, "write() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else if (0 == writeRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(Harklepipe, read_a_pipe, write failed to write any bytes);
			if (errNum)
			{
				fprintf(stderr, "write() returned errno:\t%s\n", strerror(errNum));
			}
			success = false;
		}
	}

	// DONE
	return retVal;
}
