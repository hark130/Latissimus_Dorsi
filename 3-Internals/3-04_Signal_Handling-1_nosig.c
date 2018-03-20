#include <errno.h>			// errno
#include "Harklerror.h"
#include <signal.h>			// signals
#include <stdbool.h>		// bool, true, false
#include <stdio.h>			// fprintf
#include <string.h>			// strerror
#include <sys/stat.h>		// stat
#include <sys/syscall.h>	// syscall
#include <sys/types.h>
#include <unistd.h>			// exec*()


int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	struct stat nosigFile;  // Fill this struct with stat()
	int errNum = 0;  // Store errno here upon error
	char* nosigFname = NULL;  // argv[argc - 1]
	struct sigaction sigact;  // Used to specify actions for specific signals
	int sigNum = 1;  // Signal numbers to iterate through

	// 1. INPUT VALIDATION
	fprintf(stdout, "\n");
	// 1.1. Command Line Arguments
	if (argc < 2)
	{
		fprintf(stderr, "ERROR: Too few arguments!\nUsage:\tnosig.exe </path/to/exec>\n\n");
		success = false;
	}
	else if (argv[argc - 1] == NULL)
	{
		HARKLE_ERROR(nosig, main, NULL pointer);
		success = false;
	}
	else if (*(argv[argc - 1]) == '\0')
	{
		HARKLE_ERROR(nosig, main, Empty string);
		success = false;
	}
	else
	{
		nosigFname = argv[1];
	}

	// 1.2. File type
	if (success == true)
	{
		if (-1 == stat(nosigFname, &nosigFile))
		{
			errNum = errno;
			fprintf(stderr, "ERROR: stat(%s) returned %s\n\n", nosigFname, strerror(errNum));
			success = false;
		}
		else if (!(S_ISREG(nosigFile.st_mode)))
		{
			fprintf(stderr, "ERROR: %s is not a regular file.\n\n", nosigFname);
			success = false;
		}
	}

	// 2. SIGNAL HANDLER == IGNORE
	if (success == true)
	{
		sigemptyset(&sigact.sa_mask);  // Zeroize the mask of signals which should be blocked
		sigact.sa_flags = 0;  // Can't think of any good flags to add for what I'm doing
		sigact.sa_handler = SIG_IGN;  // Apparently, instead of handling the SIGNALS, we'll just ignore them

		// Set ALL the actions
		for (sigNum = 1; sigNum <= SIGRTMAX; sigNum++)
		{
			if (sigNum == SIGKILL || sigNum == SIGSTOP)  // || sigNum == 32 || sigNum == 33)
			{
				// Skipping SIGKILL == 9	// Can't ignore
				// Skipping SIGSTOP == 19	// Can't ignore
			}
			else
			{
				// rt_sigaction() doesn't work as (barely) documented
				// Reading sigaction() source code leads me to believe that rt_sigaction wants
				// 	SIGRTMAX / 8 as the fourth argument (NOT sizeof(sigset_t) like the man page says).
				// It works.  For more details, read:
				//	https://github.com/hark130/Latissimus_Dorsi/blob/practice/Research_Documents/3-4-1-rt_sigaction.txt
				if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, SIGRTMAX / 8))
				{
					errNum = errno;
					HARKLE_ERROR(nosig, main, syscall failed);
					fprintf(stderr, "syscall(%d, %d, &sigact, NULL, %lu) set errno to %d:\t%s\n", SYS_rt_sigaction, \
																								  sigNum, \
																								  (size_t)(SIGRTMAX / 8), \
																								  errNum, \
																								  strerror(errNum));
					success = false;
					break;
				}
			}
		}
	}

	// 3. EXEC()
	if (success == true)
	{
		if (-1 == execv(nosigFname, argv + 1))
		{
			errNum = errno;
			retVal = errNum;
			fprintf(stderr, "ERROR: execv() returned %s\n\n", strerror(errNum));
			success = false;
		}
	}

	// 4. DONE
	return retVal;
}
