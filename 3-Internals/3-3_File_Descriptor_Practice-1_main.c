#include "Fileroad_Descriptors.h"
#include <fcntl.h>			// open flags
#include <stdio.h>			// fprintf
#include <string.h>			// strlen
#include <unistd.h>			// close

int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	char* filename = NULL;
	fdDetails_ptr fileStruct_ptr = NULL;

	// INPUT VALIDATION
	if (argc < 2)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Too few arguments!\n");
	}
	else if (argc > 2)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Too few arguments!\n");	
	}
	else if (argv[1] == NULL)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - NULL argument?!?!\n");		
	}
	else if (strlen(argv[1]) < 1)
	{
		fprintf(stderr, "<<<ERROR>>> - main() - Empty argument?!?!\n");	
	}
	else
	{
		// CREATE A FILE
		// Get the filename
		filename = argv[1];

		// Open the file descriptor
		fileStruct_ptr = open_fd(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );

		if (fileStruct_ptr == NULL)
		{
			fprintf(stderr, "<<<ERROR>>> - main() - open_fd() has failed!\n");	
		}
		else
		{
			// PRINT FILE DETAILS
			fprintf(stdout, "\n\nSuccessfully opened %s\n", fileStruct_ptr->filename_ptr);
			fprintf(stdout, "The file descriptor is %d\n", fileStruct_ptr->fileDesc);
			fprintf(stdout, "The size of the file is %ld bytes\n", fileStruct_ptr->fileSize);

			// WRITE TO THE FILE DESCRIPTOR
			// Implement write and call it here?
		}
	}

	// WRAP IT UP
	// Close the file descriptor
	if (fileStruct_ptr)
	{
		if (fileStruct_ptr->fileDesc > 2)
		{
			close(fileStruct_ptr->fileDesc);
		}
		// Free the struct
		free_fdDetails(&fileStruct_ptr);
	}

	// DONE
	return retVal;
}
