#include <errno.h>			// errno
#include "Harklerror.h"
#include "Memoroad.h"		// release_a_string
#include <signal.h>			// signals
#include "Signaleroad.h"	// str_signaleroad
#include <stdbool.h>		// bool, true, false
#include <stdio.h>			// fprintf
#include <stdlib.h>			// EXIT_SUCCESS
#include <string.h>			// strerror
#include <sys/stat.h>		// stat
#include <sys/types.h>
#include <sys/wait.h>		// wait
#include <unistd.h>			// exec*(), fork

// GLOBAL VARIALBES
int signalCaught = 0;


static void tester(int sig)
{
	// // LOCAL VARIABLES
	// char* temp_ptr = NULL;

	// // RESOLVE SIGNAL
	// temp_ptr = str_signaleroad(sig);

	// if (temp_ptr)
	// {
	// 	fprintf(stdout, "SIGNAL %d CAUGHT!\n%s\n", sig, temp_ptr);
	// }
	// else
	// {
	// 	HARKLE_ERROR(nosig, tester, str_signaleroad failed);
	// }

	// // CLEAN UP
	// if (temp_ptr)
	// {
	// 	if (false == release_a_string(&temp_ptr))
	// 	{
	// 		HARKLE_ERROR(nosig, tester, release_a_string failed);
	// 	}
	// }

	// Save the signal caught
	signalCaught = sig;

	// DONE
	return;
}


static void silencer(int sig)
{
	// fprintf(stdout, "signal %d silenced", sig);
	// Save the signal caught
	signalCaught = sig;
	return;
}


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
	struct sigaction sigact;  // Used to specify actions for specific signals
	int sigNum = 1;  // Signal numbers to iterate through
	char* signal_ptr = NULL;  // Allocate a description of the signal caught

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
	if (success == true)
	{
		sigemptyset(&sigact.sa_mask);  // Zeroize the mask of signals which should be blocked
		sigact.sa_flags = 0;  // Can't think of any good flags to add for what I'm doing
		// sigact.sa_handler = tester;  // I want to see the signals go through first
		sigact.sa_handler = silencer;  // This is the real handler for this program

		// Set all the actions
		for (sigNum = 1; sigNum <= 64; sigNum++)
		{
			if (sigNum == SIGKILL || sigNum == SIGSTOP || sigNum == SIGCHLD || sigNum == 32 || sigNum == 33)
			{
				// Skipping SIGKILL == 9
				// Skipping SIGSTOP == 19
			}
			else
			{
				if (-1 == sigaction(sigNum, &sigact, NULL))
				{			
					HARKLE_ERROR(nosig, main, sigaction failed);
					fprintf(stderr, "sigaction(%d, &sigact, NULL) failed\n", sigNum);
					success = false;
					break;
				}
			}
		}
	}

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
				else
				{
					_exit(EXIT_SUCCESS);
				}
				break;
			default:  // Parent
				// tempPID = wait(&statLoc);
				tempPID = waitpid(forkPID, &statLoc, 0);

				if (tempPID == -1)
				{
					HARKLE_ERROR(nosig, main, waitpid failed);
					success = false;
				}
				else if (forkPID != tempPID)
				{
					HARKLE_ERROR(nosig, main, waitpid caught the wrong PID);
					fprintf(stderr, "Expecting PID %d but wait() caught PID %d\n", forkPID, tempPID);  // DEBUGGING
					success = false;
				}

				fprintf(stdout, "%s returned %d.\n", nosigFname, statLoc);

				// RESOLVE SIGNAL CAUGHT
				if (signalCaught)
				{
					signal_ptr = str_signaleroad(signalCaught);

					if (signal_ptr)
					{
						fprintf(stdout, "%s (%d) SIGNAL CAUGHT!\n", signal_ptr, signalCaught);
					}
					else
					{
						HARKLE_ERROR(nosig, main, str_signaleroad failed);
					}
				}

				// RESOLVE CHILD STATUS
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
					fprintf(stdout, "%s terminated with signal number %d - %s.\n", nosigFname, WTERMSIG(statLoc), strsignal(WTERMSIG(statLoc)));
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

	// 5. CLEAN UP
	if (signal_ptr)
	{
		if (false == release_a_string(&signal_ptr))
		{
			HARKLE_ERROR(nosig, main, release_a_string failed);
		}
	}

	// 5. DONE
	return retVal;
}