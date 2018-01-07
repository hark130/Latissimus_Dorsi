#include <stdio.h>
#include <dlfcn.h>
// int func_4_2_1(int a, int b);

int main(void)
{
	// LOCAL VARIABLES
	void* handle = NULL;  // Handle to shared library
	int(*main421)(int, int) = NULL;  // Function pointer to func_4_2_1()
	int retVal = 0;
	int a = 4;
	int b = 2;

	// LOAD FUNCTION POINTER
	puts("Pre-Function Load");  // DEBUGGING

	handle = dlopen("./lib421PICpractice.so", RTLD_NOW | RTLD_GLOBAL);
	if (!handle)
	{
		fprintf(stderr, "Unable to open lib: %s\n", dlerror());
		retVal = -1;
	}
	else
	{
		puts("\tLibrary opened!");  // DEBUGGING
		main421 = dlsym(handle, "func_4_2_1");

		if (!main421)
		{
			fprintf(stderr, "\tUnable to get symbol!\n");
			retVal = -2;
		}
		else
		{
			puts("\tFunction loaded!");  // DEBUGGING
		}
	}
	
	puts("Post-Function Load\n");  // DEBUGGING	

	// CALL FUNCTION POINTER
	if (main421)
	{		
		puts("Pre-Function Call");  // DEBUGGING

		printf("\tCalling func_4_2_1() with %d and %d.\n", a, b);  // DEBUGGING
		retVal = main421(a, b);
		printf("\tfunc_4_2_1() returned %d.\n", retVal);  // DEBUGGING

		puts("Post-Function Call\n");  // DEBUGGING
	}

	// DONE
	return retVal;
}
