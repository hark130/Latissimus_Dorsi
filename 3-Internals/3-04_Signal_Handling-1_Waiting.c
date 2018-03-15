#include <stdio.h>			// fprintf
#include <unistd.h>			// sleep

int main(void)
{
	printf("Waiting...");
	while (1)
	{
		sleep(1);
		printf(".");
		// PRO TIP: sleep() doesn't link it if you print to stdout without using a newling '\n'
		//	You have to flush the stream?!
		fflush(stdout);
	}

	return 0;
}