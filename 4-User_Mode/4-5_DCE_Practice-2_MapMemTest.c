#include "Map_Memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef BUFF_SIZE
#define BUFF_SIZE 512
#endif  // BUFF_SIZE

int main(void)
{
	// LOCAL VARIABLES
	int retVal = 0;
	mapMem_ptr memStruct_ptr = NULL;
	char* tempChar_ptr = NULL;

	// ALLOCATE THE STRUCT
	memStruct_ptr = create_mapMem_ptr();

	if (memStruct_ptr)
	{
		memStruct_ptr->memSize = BUFF_SIZE;
		memStruct_ptr->fileMem_ptr = (char*)calloc(memStruct_ptr->memSize, sizeof(char));

		if (memStruct_ptr->fileMem_ptr)
		{
			tempChar_ptr = strncpy(memStruct_ptr->fileMem_ptr, "This is a test of Map_Memory.h!", memStruct_ptr->memSize);

			if (tempChar_ptr == memStruct_ptr->fileMem_ptr)
			{
				fprintf(stdout, "Successfully copied the following string:\t%s\n", memStruct_ptr->fileMem_ptr);
			}
			else
			{
				fprintf(stderr, "Failed to properly strncpy a string into the mapMem_ptr->fileMem_ptr!\n");
				retVal = -3;
			}
		}
		else
		{
			fprintf(stderr, "calloc() failed to allocate into mapMem_ptr!\n");
			retVal = -2;
		}
	}
	else
	{
		fprintf(stderr, "create_mapMem_ptr() failed to allocate!\n");
		retVal = -1;
	}

	// FREE THE STRUCT
	// void free_struct(mapMem_ptr** oldStruct_ptr);
	free_struct(&memStruct_ptr);
	if (memStruct_ptr)
	{
		fprintf(stderr, "free_struct() failed to free()!\n");
		retVal = -4;

		if (memStruct_ptr->fileMem_ptr)
		{
			fprintf(stderr, "free_struct() also failed to free() the struct's char pointer!\n");
		}
		if (memStruct_ptr->memSize)
		{
			fprintf(stderr, "free_struct() also failed to clear the struct's memSize!\n");
		}
	}

	return retVal;
}
