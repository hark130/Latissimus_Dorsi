/*
	4-5_DCE_Practice 1 attempts to replicate a concept read in "Learning 
		Linux Binary Analysis" Ch 2. (see: 4 README)
		
	"Let us pretend we are an attacker and we want to infect a 32-bit program 
	that calls puts() to print Hello World. Our goal is to hijack puts() so 
	that it calls evil_puts()""

	This is the source of the binary to hijack.
 */

#include <stdio.h>

int main(void)
{
	puts("Hello World\n");
	return 0;
}

