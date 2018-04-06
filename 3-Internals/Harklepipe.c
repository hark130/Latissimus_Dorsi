#include <errno.h>			// errno
#include "Harklepipe.h"		// HPIPE_READ, HPIPE_WRITE
#include "Harklerror.h"		// HARKLE_ERROR
#include <stdbool.h>		// bool, true, false
#include <stdio.h>			// fprintf()
#include <string.h>			// strerror
#include <unistd.h>			// pipe(), pipe2()


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
			#ifdef _GNU_SOURCE
			retVal = pipe2(emptyPipes, flags);
			#else
			retVal = pipe(emptyPipes);
			#endif  // _GNU_SOURCE
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
			fprintf(stderr, "Function call failed with:\t%s\n", strerror(errNum));
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
		if (writeFD != emptyPipes[HPIPE_READ])
		{
			close(emptyPipes[HPIPE_WRITE]);
			emptyPipes[HPIPE_WRITE] = writeFD;
		}
	}

	// DONE
	return retVal;
}
