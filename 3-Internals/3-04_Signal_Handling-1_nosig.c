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

		// Set all the actions
		for (sigNum = 1; sigNum <= SIGRTMAX; sigNum++)
		// for (sigNum = 1; sigNum <= 64; sigNum++)
		{
			if (sigNum == SIGKILL || sigNum == SIGSTOP)  // || sigNum == 32 || sigNum == 33)
			{
				// Skipping SIGKILL == 9	// Can't ignore
				// Skipping SIGSTOP == 19	// Can't ignore
			}
			else
			{
				/* IMPORTANT NOTE */
				// I decided to transition from sigaction() to syscall(rt_sigaction) so that
				//	I could ignore (SIG_IGN) signals that exceeded 31.  Here's some of what I learned:
				// 	1. rt_sigaction, the underpinings of sigaction() can handle larger sigset_t sizes (SEE: sigact.sa_mask)
				//	2. rt_sigaction's fourth argument "is currently required to have the value sizeof(sigset_t)" (http://man7.org/linux/man-pages/man2/sigaction.2.html)
				//  3. On my system, sizeof(sigset_t) and sizeof(sigact.sa_mask) both resolve to 128
				//  4. syscall(13, 1, &sigact, NULL, 128) == EINVAL
				//  5. An strace of sigaction(1, &sigact, NULL) reveals rt_sigaction(SIGHUP, {SIG_IGN, [], SA_RESTORER, 0x7fbfdcedd4b0}, NULL, 8)
				//		(SEE: Latissimus_Dorsi/3-Internals/Error_Output/nosig03-EINVAL-sigaction-strace.txt if it still exists)
				//	6. An strace of syscall(13, 1, &sigact, NULL, 128) reveals rt_sigaction(SIGHUP, {SIG_IGN, [], 0}, NULL, 128)
				//		(SEE: Latissimus_Dorsi/3-Internals/Error_Output/nosig03-EINVAL-rt_sigaction-strace.txt if it still exists)
				//	7. Observing this behavior leads me to the following conclusions
				//		A. sigaction() has added the SA_RESTORER flag to the rt_sigaction() system call
				//		B. SA_RESTORER - "Not intended for application use.  This flag is used by C libraries to indicate 
				//			that the sa_restorer field contains the address of a "signal trampoline".
				//		C. This means that 0x7fbfdcedd4b0 is likely the memory address of the "signal trampoline" sigaction is using
				//			(SEE: SIGRETURN(2) for more on trampolines)
				//		D. sigaction() is using the value of 8 as the value for the fourth argument.
				//	8. Observing this behavior has raised the following questions:
				//		A. Where is sigaction() getting "8" from?  If I had 3 guesses, here they are (in order):
				//			i. The size of the memory address storing the signal number?  	sizeof(void*) == 8
				//			ii. The size of &sigact?  										sizeof(&sigact) == 8
				//			iii. sizeof(sigset_t) / 16?										sizeof(sigset_t) == 128
				//				This one is far fetched but it's the only calculation involving sizeof(sigset_t) that works.
				//				After all, "This argument is currently required to have the value sizeof(sigset_t)"
				// 	9. Now we go deeper.  Supposedly, one of the advantages to programming in Linux is that it's open source.  Let's
				//		put that to the test.
				//		Q1: Where does sigaction() get the value of 8 from?
				//		A1: Source code for sigaction.c calculates "_NSIG / 8" (https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/sigaction.c.html)
				//		Q2: What is "_NSIG"?
				//		A2: (64 + 1) /* Biggest signal number + 1 (including real-time signals). */ /* From <asm/signal.h> */
				//		Q3: Where does <asm/signal.h> get "_NSIG" from?  (I looked on my local installation at /usr/include/asm-generic/signal.h)
				//		A3: #define _NSIG 64
				//		Q4: Can I utilize _NSIG 64 in my code?
				//		A4: I saw lots of warnings NOT to:
				//			i. Regarding SIGRTMIN and SIGRTMAX, 
							 /* These should not be considered constants from userland. */ 
							 // (/usr/include/asm-generic/signal.h)
				//			ii. Regarding __SIGRTMIN and __SIGRTMAX, 
							 /* These are the hard limits of the kernel.  
								These values should not be used directly at user level. */ 
							 // (/usr/include/x86_64-linux-gnu/bits/signum.h)
				//			iii. Regarding __SIGRTMIN and __SIGRTMAX, 
							 /* Not all systems support real-time signals.  bits/signum.h indicates
   								that they are supported by overriding __SIGRTMAX to a value greater
   								than __SIGRTMIN.  These constants give the kernel-level hard limits,
   								but some real-time signals may be used internally by glibc.  Do not
   								use these constants in application code; use SIGRTMIN and SIGRTMAX
   								(defined in signal.h) instead.  */ 
   							 // (https://code.woboq.org/userspace/glibc/bits/signum-generic.h.html)
				// FINAL CONCLUSION:  The answer to 8.A. is "_NSIG / 8".  What should *I* use?  It looks like "SIGRTMAX / 8".
				// SEE: https://github.com/hark130/Latissimus_Dorsi/blob/practice/Research_Documents/3-4-1-rt_sigaction.txt



				// if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, sizeof(sigset_t)))  // == EINVAL (because sizeof(sigset_t) == 128 on my machine)
				// if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, sizeof(sigact.sa_mask)))  // == EINVAL (because sizeof(sigset_t) == 128 on my machine)
				// if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, 8))  // Now, I'm just trying to replicate the rt_sigaction() call I see in strace
				// if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, sizeof(sigset_t) / 16))  // Now, I'm just trying to replicate the rt_sigaction() call I see in strace
				if (-1 == syscall(SYS_rt_sigaction, sigNum, &sigact, NULL, SIGRTMAX / 8))  // Now, I'm just trying to replicate the rt_sigaction() call I see in strace
				// if (-1 == sigaction(sigNum, &sigact, NULL))
				{
					errNum = errno;
					HARKLE_ERROR(nosig, main, syscall failed);
					fprintf(stderr, "syscall(%d, %d, &sigact, NULL, %lu) set errno to %d:\t%s\n", SYS_rt_sigaction, sigNum, sizeof(sigset_t), errNum, strerror(errNum));
					// HARKLE_ERROR(nosig, main, sigaction failed);
					// fprintf(stderr, "sigaction(%d, &sigact, NULL) set errno to %d:\t%s\n", sigNum, errNum, strerror(errNum));
					success = false;
					break;
				}
			}
		}
		// fprintf(stdout, "Pointer size is:\t%lu\n", sizeof(&sigact));  // DEBUGGING		
		// fprintf(stdout, "SIGNAL size is:\t%lu\n", sizeof(sigNum));  // DEBUGGING
	}

	// 2.5 WAIT HERE... VALGRIND TESTING OF THE SYST_RT_SIGACTION SYSTEM CALL
	// int i = 0;
	// printf("Waiting...");
	// while (1)
	// {
	// 	sleep(1);
	// 	printf(".");
	// 	// PRO TIP: sleep() doesn't link it if you print to stdout without using a newling '\n'
	// 	//	You have to flush the stream?!
	// 	fflush(stdout);
	// 	// PURELY FOR VALGRIND'S SAKE
	// 	if (++i > 10) { printf("\n"); fflush(stdout); break; }
	// }

	// 3. DON'T CALL FORK... JUST EXEC() LIKE NOHUP
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
