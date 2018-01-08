/*
	4-5_DCE_Practice 1 attempts to replicate a concept read in 
		"Function Hooking - Part I" (see: 4 README)
		
	"The LD_PRELOAD environment variable is used to specify a shared 
	library that is to be loaded first by the loader. Loading our shared 
	library first enables us to intercept function calls and using the 
	dynamic loader API we can bind the originally intended function to 
	a function pointer and pass the original arguments through it, 
	effectively wrapping the function call."

	This is the source of the binary to be fooled.
 */

#include <stdio.h>

int main(void)
{
	puts("Hello World\n");
	return 0;
}

