#include <stdio.h>


int main(int argc, char** argv)
{
	// LOCAL VARIABLES
	char** temp_arr = argv;

	// INPUT VALIDATION
	if (temp_arr)
	{
		temp_arr++;
		if (*temp_arr)
		{
			// DO IT
			while (*temp_arr)
			{
				fprintf(stdout, "%s ", *temp_arr);
				temp_arr++;
			}
			fprintf(stdout, "\n");
		}
	}

	// DONE
	return 0;
}
