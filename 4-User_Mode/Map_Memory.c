#include "Map_Memory.h"
#include <stdbool.h>		// bool, true, false
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>		// mmap, msync, munmap
#include <sys/stat.h>		// fstat
#include <fcntl.h>			// open
#include <unistd.h>			// close

#ifndef MAX_TRIES
// MACRO to limit repeated allocation attempts
#define MAX_TRIES 3
#endif  // MAX_TRIES


mapMem_ptr create_mapMem_ptr(void)
{
	// LOCAL VARIABLES
	mapMem_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while(numTries < MAX_TRIES && retVal == NULL)
	{
		retVal = (mapMem_ptr)calloc(1, sizeof(mapMem));
		numTries++;
	}

	// DONE
	return retVal;
}


/*
	Purpose - Map a file's contents to memory
	Input - Filename to map into memory
	Output - Pointer to a struct mappedMemory
	Notes:
		mapMem_ptr must be free()'d by the calling function
		mapMem_ptr->fileMem_ptr must be free()'d by the calling function
 */
mapMem_ptr map_file(const char* filename)
{	
	// LOCAL VARIABLES
	mapMem_ptr retVal = NULL;
	int fileDesc = -1;  // Set to error value by default for the purposes of clean-up
	struct stat fileStat;
	
	// INPUT VALIDATION
	if (NULL == filename)
	{
		fprintf(stderr, "map_file() - filename is NULL\n");
		return retVal;
	}
	else if (0 == strlen(filename))
	{
		fprintf(stderr, "map_file() - filename is empty\n");
		return retVal;
	}
	else
	{	
		// MAP FILE INTO MEMORY
		// 1. Create mapMem_ptr
		retVal = create_mapMem_ptr();
		if (NULL == retVal)
		{
			fprintf(stderr, "map_file() - create_mapMem_ptr() returned NULL\n");
		}
		else
		{
			// 2. Get file descriptor for filename
			fileDesc = open(filename, O_RDWR);
			if (0 > fileDesc)
			{
				fprintf(stderr, "map_file() - unable to open '%s'\n", filename);
			}
			else
			{
				// 3. Verify the file descriptor is a regular file
				// 3.1. Get the file status
				if (0 != fstat(fileDesc, &fileStat))
				{
					fprintf(stderr, "map_file() - unable to fstat '%s'\n", filename);
				}
				else
				{
					// 3.2. Verify the file status indicates this is a regular file
					if (!S_ISREG(fileStat.st_mode))
					{
						fprintf(stderr, "map_file() - '%s' is not a regular file\n", filename);
					}
					else
					{
						if (0 >= fileStat.st_size)
						{
							fprintf(stderr, "map_file() - Invalid size for '%s'\n", filename);
						}
						else
						{
							// 3.3. Populate mapMem struct with file size
							retVal->memSize = fileStat.st_size;
							// 4. Map the file descriptor into memory
							/*
								void * mmap (void *addr,
											 size_t len,
											 int prot,
											 int flags,
											 int fd,
											 off_t offset);
							 */
							retVal->fileMem_ptr = mmap(NULL, \
														   retVal->memSize, \
														   PROT_READ | PROT_WRITE | PROT_EXEC, \
														   MAP_SHARED, \
														   fileDesc, \
														   0);
							if (NULL == retVal->fileMem_ptr)
							{
								fprintf(stderr, "map_file() - mmap failed to map file descriptor %d into memory\n", fileDesc);
							}
							else
							{
								fprintf(stdout, "map_file() appears to have succeeded!\n");
								retVal->memType = MM_TYPE_MMAP;
							}
						}
					}
				}
			}
		}
	}
	
	// CLEANUP
	// 1. Check the struct
	if (retVal)
	{
		// Struct was allocated
		if (NULL == retVal->fileMem_ptr || retVal->memSize <= 0)
		{
			// Something went wrong while populating the struct
			free_struct(&retVal);
		}
	}
	// 2. Check the file descriptor
	if (fileDesc > -1)
	{
		// Close it
		close(fileDesc);
		fileDesc = -1;
	}

	// DONE
	return retVal;
}


/*
	Purpose - Unmap a file's contents from memory
	Input
		memStruct_ptr - mappedMemory pointer
		syncMem - if true, msync to file
	Output - true on *total* success, otherwise false
	Notes:
		Calling function is responsible for free()'ing memStruct_ptr on success
 */
bool unmap_file(mapMem_ptr memStruct_ptr, bool syncMem)
{
	// LOCAL VARIABLES
	bool retVal = false;
	
	// INPUT VALIDATION
	if (NULL != memStruct_ptr)
	{
		if (NULL != memStruct_ptr->fileMem_ptr && memStruct_ptr->memSize > 0)
		{
			// 1. Sync memory with the file IAW syncMem
			if (true == syncMem)
			{
				if (msync(memStruct_ptr->fileMem_ptr, memStruct_ptr->memSize, MS_INVALIDATE | MS_SYNC))
				{
					fprintf(stderr, "unmap_file() - unable to msync mem to file\n");
				}
				else
				{
					// fprintf(stdout, "unmap_file() appears to have successfully msync()'d mem to file!\n");
				}
			}
			
			// 2. Unmap mem
			if (munmap(memStruct_ptr->fileMem_ptr, memStruct_ptr->memSize))
			{
				fprintf(stderr, "unmap_file() - munmap() failed\n");
			}
			else
			{
				// fprintf(stdout, "unmap_file() appears to have successfully unmapped memory!\n");
				retVal = true;
			}
		}
	}	
	
	// DONE
	return retVal;
}


void free_struct(mapMem_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	char* tempBuff = NULL;
	size_t tempSize = 0;
	mapMem_ptr tempStruct_ptr = NULL;

	// INPUT VALIDATION
	if (NULL != oldStruct_ptr)
	{
		if (NULL != *oldStruct_ptr)
		{
			// 1. FREE/CLEAR STRUCT CONTENTS
			// Get current values
			tempStruct_ptr = (mapMem_ptr)*oldStruct_ptr;
			tempBuff = tempStruct_ptr->fileMem_ptr;
			tempSize = tempStruct_ptr->memSize;

			switch(tempStruct_ptr->memType)
			{
				// Free memory (as appropriate, if appropriate, in appropriate ways)
				case MM_TYPE_CAVE:
					// Represents an offset into memory not owned by this struct
					// Do *NOT* attempt to free this memory
					break;
				case MM_TYPE_MMAP:
					if (!unmap_file(tempStruct_ptr, true))
					{
						fprintf(stderr, "free_struct() - unmap_file() failed\n");
					}
					break;
				case MM_TYPE_HEAP:
					// Treat malloc/calloc mem types same as default
				default:
					// Memset
					if (tempSize > 0 && NULL != tempBuff)
					{
						memset(tempBuff, 0x0, tempSize);
					}
					// Free fileMem_ptr
					if (NULL != tempBuff)
					{
						free(tempBuff);
					}
					break;
			}
			
			// Clear the pointer variables
			tempBuff = NULL;
			tempStruct_ptr->fileMem_ptr = NULL;
			// Clear memSize
			tempSize = 0;
			tempStruct_ptr->memSize = 0;
			// Clear memType
			tempStruct_ptr->memType = 0;
			
			// 2. FREE/CLEAR STRUCT
			// Free the struct pointer
			free((void*)*oldStruct_ptr);
			// Clear the struct pointer
			tempStruct_ptr = NULL;
			*oldStruct_ptr = NULL;
		}
	}

	// DONE
	return;
}


bool validate_struct(mapMem_ptr checkThis_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;

	// INPUT VALIDATION
	if (NULL == checkThis_ptr)
	{
		retVal = false;
	}
	else
	{
		// CHECK THE STRUCT
		// 1. fileMem_ptr
		if (NULL == checkThis_ptr->fileMem_ptr)
		{
			retVal = false;
			fprintf(stderr, "This mappedMemory struct contains a NULL pointer!");
		}
		else if (0 >= checkThis_ptr->memSize)
		{
			retVal = false;
			fprintf(stderr, "This mappedMemory struct contains an invalid memory size!");	
		}
		else if (MM_TYPE_CAVE != checkThis_ptr->memType && \
			     MM_TYPE_HEAP != checkThis_ptr->memType && \
			     MM_TYPE_MMAP != checkThis_ptr->memType)
		{
			retVal = false;
			fprintf(stderr, "This mappedMemory struct contains an invalid memory type!");			
		}
	}

	// DONE
	// puts("Finished validate_struct()");  // DEBUGGING
	return retVal;
}

/*
	Refs:
		https://www.safaribooksonline.com/library/view/linux-system-programming/0596009585/ch04s03.html
 */



