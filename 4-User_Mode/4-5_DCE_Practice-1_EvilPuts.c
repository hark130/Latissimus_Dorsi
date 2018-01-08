/*
	4-5_DCE_Practice 1 attempts to replicate a concept read in 
		"Function Hooking - Part I" (see: 4 README)
		
	"The LD_PRELOAD environment variable is used to specify a shared 
	library that is to be loaded first by the loader. Loading our shared 
	library first enables us to intercept function calls and using the 
	dynamic loader API we can bind the originally intended function to 
	a function pointer and pass the original arguments through it, 
	effectively wrapping the function call."

	This is the source of the replacement code for puts().
 */

#include <sys/syscall.h>	// write, SYS_write
#include <string.h>			// strlen

// int puts(const char *s);
int puts(const char *s)
// int evil_puts(const char *s)
{
	int retVal;
	char hijackMsg[] = { "HAHA puts() has been hijacked!\n" };
	retVal = (int)syscall(SYS_write, 1, hijackMsg, strlen(hijackMsg));
	return retVal;
}
