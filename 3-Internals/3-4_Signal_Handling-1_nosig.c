#include <errno.h>		// errno
#include "Harklerror.h"
#include <signal.h>		// signals
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fprintf
#include <string.h>		// strerror
#include <sys/stat.h>	// stat
#include <sys/types.h>
#include <sys/wait.h>	// wait
#include <unistd.h>		// exec*(), fork


int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	struct stat nosigFile;  // Fill this struct with stat()
	int errNum = 0;  // Store errno here upon error
	char* nosigFname = NULL;  // argv[argc - 1]
	pid_t forkPID = 0;  // Store the return value from fork() here
	pid_t tempPID = 0;  // Return value for wait()
	int statLoc = 0;  // [OUT] parameter for wait()

	// 1. INPUT VALIDATION
	fprintf(stdout, "\n");
	// 1.1. Command Line Arguments
	if (argc < 2)
	{
		fprintf(stderr, "ERROR: Too few arguments!\nUsage:\tnosig.exe </path/to/exec>\n\n");
		success = false;
	}
	else if (argc > 2)
	{
		fprintf(stderr, "ERROR: Too many arguments!\nUsage:\tnosig.exe </path/to/exec>\n\n");
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
		nosigFname = argv[argc - 1];
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

	// 2. CREATE ARGUMENT LIST
	////////////////////////////// IMPLEMENT LATER //////////////////////////////
	/* TEMP */
	char* argList[] = { nosigFname, NULL };  // Temporary solution

	// 3. SIGNAL HANDLER
	////////////////////////////// IMPLEMENT LATER //////////////////////////////

	// 4. CALL IT
	if (success == true)
	{
		switch (forkPID = fork())
		{
			case (-1):
				errNum = errno;
				HARKLE_ERROR(nosig, main, fork failed);
				fprintf(stderr, "ERROR: fork() returned %s\n\n", strerror(errNum));
				success = false;
			case (0):  // Child
				if (-1 == execv(nosigFname, argList))
				{
					errNum = errno;
					retVal = errNum;
					fprintf(stderr, "ERROR: execv() returned %s\n\n", strerror(errNum));
				}
				break;
			default:  // Parent
				tempPID = wait(&statLoc);

				if (forkPID != tempPID)
				{
					HARKLE_ERROR(nosig, main, wait caught the wrong PID);
					success = false;
				}

				fprintf(stdout, "%s returned %d.\n", nosigFname, statLoc);
		
				if (WIFEXITED(statLoc))
				{
					fprintf(stdout, "%s returned normally.\n", nosigFname);
				}
				else
				{
					fprintf(stdout, "%s did not return normally.\n", nosigFname);
				}

				if (WEXITSTATUS(statLoc))
				{
					fprintf(stdout, "%s returned %d from main().\n", nosigFname, WEXITSTATUS(statLoc));
				}

				if (WIFSIGNALED(statLoc))
				{
					fprintf(stdout, "%s terminated due to the receipt of a signal that was not caught.\n", nosigFname);
				}

				if (WTERMSIG(statLoc))
				{
					fprintf(stdout, "%s terminated with signal number %d.\n", nosigFname, WTERMSIG(statLoc));
				}

				if (WIFSTOPPED(statLoc))
				{
					fprintf(stdout, "%s is currently stopped.\n", nosigFname);
				}

				if (WSTOPSIG(statLoc))
				{
					fprintf(stdout, "%d is the number of the signal that caused the %s to stop.\n", WSTOPSIG(statLoc), nosigFname);
				}

				if (WIFCONTINUED(statLoc))
				{
					fprintf(stdout, "%s continued from a job control stop.\n", nosigFname);
				}

				fprintf(stdout, "\n");
		}
	}

	// 5. DONE
	return retVal;
}