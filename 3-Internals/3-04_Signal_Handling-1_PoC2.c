#include "Harklerror.h"
#include "Memoroad.h"
#include "Signaleroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// GLOBAL VARIABLE
int signalCaught = 0;

void sig_handler(int signo)
{
	signalCaught = signo;
}

int main(void)
{
	// LOCAL VARIABLES
	int sigNum = 0;
	bool success = true;
	struct sigaction sigact;  // Used to specify actions for specific signals

	// 3. SIGNAL HANDLER
	if (success == true)
	{
		sigemptyset(&sigact.sa_mask);  // Zeroize the mask of signals which should be blocked
		sigact.sa_flags = 0;  // Can't think of any good flags to add for what I'm doing
		sigact.sa_handler = sig_handler;  // This is the signal handler for this program

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
					HARKLE_ERROR(PoC2, main, sigaction failed);
					fprintf(stderr, "sigaction(%d, &sigact, NULL) failed\n", sigNum);
					success = false;
					break;
				}
			}
		}
	}
  	
  	while(1)
  	{		
		if (signalCaught)
		{
			// RESOLVE SIGNAL CAUGHT
			// 1. Convert signal number to signal string
			signal_ptr = str_signaleroad(signalCaught);

			if (signal_ptr)
			{
				// 2. Print the string details
				fprintf(stdout, "\n%s (%d) SIGNAL CAUGHT!\n", signal_ptr, signalCaught);

				// 3. Free the string
				if (false == release_a_string(&signal_ptr))
				{
					HARKLE_ERROR(PoC2, main, release_a_string failed);
				}
			}
			else
			{
				HARKLE_ERROR(nosig, main, str_signaleroad failed);
			}

			// 4. Set signalCaught back to 0
			signalCaught = 0;
		}
    	sleep(1);
  	}

  	return 0;
}
