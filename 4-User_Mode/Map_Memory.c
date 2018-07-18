#include <errno.h>								// errno
#include "Harklerror.h"							// HARKLE_ERROR
#include "Map_Memory.h"							// HMAP_SHARED_VALIDATE
#include <stdbool.h>							// bool, true, false
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>							// mmap, msync, munmap
#include <sys/stat.h>							// fstat
#include <inttypes.h>							// intmax_t
#include <fcntl.h>								// open
#include <unistd.h>								// close

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


mapMem_ptr map_anon(size_t length, int prot, int flags)
{
	// LOCAL VARIABLES
	mapMem_ptr retVal = NULL;
	int errNum = 0;  // Store errno here
	int minProt = PROT_EXEC | PROT_READ | PROT_WRITE;  // Minimum protections
	int minFlags = MAP_SHARED | HMAP_SHARED_VALIDATE | MAP_PRIVATE;  // Minimum flags
	int actualFlags = flags | MAP_ANONYMOUS;  // Update the parameter flags with MAP_ANONYMOUS
	errno = 0;
	bool success = true;  // Make this false if anything fails

	// INPUT VALIDATION
	if (length < 1)
	{
		HARKLE_ERROR(Map_Memory, map_anon, Invalid length);
		success = false;
	}
	else if (!(prot & minProt) && prot != PROT_NONE)
	{
		HARKLE_ERROR(Map_Memory, map_anon, Minimum protections missing);
		success = false;
	}
	else if (!(flags & minFlags) && flags != 0)
	{
		HARKLE_ERROR(Map_Memory, map_anon, Minimum flags missing);
		success = false;
	}

	// MAP IT
	// 1. Create mapMem_ptr
	retVal = create_mapMem_ptr();
	if (NULL == retVal)
	{
		HARKLE_ERROR(Map_Memory, map_anon, create_mapMem_ptr failed);
		success = false;
	}
	else
	{
		// 2. Determine read-only permissions
		if (PROT_WRITE != (prot & PROT_WRITE))
		{
			retVal->readOnly = true;
		}
		else
		{
			retVal->readOnly = false;
		}

		// 3. Store size
		retVal->memSize = length;
	}
	
	// 4. Map memory
	if (true == success)
	{
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
								   prot, \
								   actualFlags, \
								   -1, \
								   0);

		if (NULL == retVal->fileMem_ptr || MAP_FAILED == retVal->fileMem_ptr)
		{
			errNum = errno;
			retVal->fileMem_ptr = NULL;
			HARKLE_ERROR(Map_Memory, map_anon, mmap failed);
			HARKLE_ERRNO(Map_Memory, mmap, errNum);
		}
		else
		{
			retVal->memType = MM_TYPE_HEAP;
		}
	}
	
	// CLEANUP
	if (false == success && retVal)
	{
		// Something went wrong while populating the struct
		free_struct(&retVal);
	}

	// DONE
	return retVal;
}


mapMem_ptr map_file(const char* filename)
{
	// LOCAL VARIABLES
	mapMem_ptr retVal = NULL;

	// FUNCTION CALL
	retVal = map_file_mode(filename, O_RDWR);

	// DONE
	return retVal;
}


mapMem_ptr map_file_mode(const char* filename, int flags)
{	
	// LOCAL VARIABLES
	mapMem_ptr retVal = NULL;
	int fileDesc = -1;  // Set to error value by default for the purposes of clean-up
	struct stat fileStat;
	int minFlags = O_RDONLY | O_WRONLY | O_RDWR;
	int errNum = 0;  // Store errno here
	errno = 0;

	// fprintf(stdout, "Min Flags:\t%d\n", minFlags);  // DEBUGGING
	// fprintf(stdout, "int flags:\t%d\n", flags);  // DEBUGGING	
	// INPUT VALIDATION
	if (NULL == filename)
	{
		HARKLE_ERROR(Map_Memory, map_file_mode, filename is NULL);
		// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - filename is NULL!\n");
		// return retVal;
	}
	else if (0 == strlen(filename))
	{
		HARKLE_ERROR(Map_Memory, map_file_mode, filename is empty);
		// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - filename is empty!\n");
		// return retVal;
	}
	else if (!(flags & minFlags) && flags != 0)
	{
		HARKLE_ERROR(Map_Memory, map_file_mode, minimum flags missing);
		// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file_mode() - minimum flags missing!\n");
		// return retVal;
	}
	else
	{	
		// MAP FILE INTO MEMORY
		// 1. Create mapMem_ptr
		retVal = create_mapMem_ptr();
		if (NULL == retVal)
		{
			HARKLE_ERROR(Map_Memory, map_file_mode, create_mapMem_ptr failed);
			// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - create_mapMem_ptr() returned NULL!\n");
		}
		else
		{
			// 2. Get file descriptor for filename
			// Determine read-only permissions
			if (O_RDONLY == (flags & O_RDONLY) || O_RDONLY == flags)
			{
				retVal->readOnly = true;
			}
			else
			{
				retVal->readOnly = false;
			}

			// fileDesc = open(filename, flags);
			fileDesc = open(filename, O_RDWR);
			if (0 > fileDesc)
			{
				HARKLE_ERROR(Map_Memory, map_file_mode, Unable to open filename);
				// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - unable to open '%s'!\n", filename);
			}
			else
			{
				// 3. Verify the file descriptor is a regular file
				// 3.1. Get the file status
				if (0 != fstat(fileDesc, &fileStat))
				{
					HARKLE_ERROR(Map_Memory, map_file_mode, Unable to fstat filename);
					// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - unable to fstat '%s'!\n", filename);
				}
				else
				{
					// 3.2. Verify the file status indicates this is a regular file
					if (!S_ISREG(fileStat.st_mode))
					{
						HARKLE_ERROR(Map_Memory, map_file_mode, Not a regular file);
						// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - '%s' is not a regular file!\n", filename);
					}
					else
					{
						if (0 >= fileStat.st_size)
						{
							HARKLE_ERROR(Map_Memory, map_file_mode, Invalid file size);
							// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - Invalid size of %jd for '%s'!\n", (intmax_t)fileStat.st_size, filename);
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
							if (true == retVal->readOnly)
							{
								retVal->fileMem_ptr = mmap(NULL, \
														   retVal->memSize, \
														   PROT_READ | PROT_WRITE | PROT_EXEC, \
														   MAP_SHARED, \
														   fileDesc, \
														   0);
							}
							else
							{
								retVal->fileMem_ptr = mmap(NULL, \
														   retVal->memSize, \
														   PROT_READ | PROT_WRITE | PROT_EXEC, \
														   MAP_SHARED, \
														   fileDesc, \
														   0);
							}
							if (NULL == retVal->fileMem_ptr || MAP_FAILED == retVal->fileMem_ptr)
							{
								errNum = errno;
								retVal->fileMem_ptr = NULL;
								HARKLE_ERROR(Map_Memory, map_file_mode, mmap failed);
								// fprintf(stderr, "<<<ERROR>>> - Map_Memory - map_file() - mmap failed to map file descriptor %d into memory!\n", fileDesc);
								HARKLE_ERRNO(Map_Memory, mmap, errNum);
							}
							else
							{
								// fprintf(stdout, "<<<SUCCESS>>> - Map_Memory - map_file() appears to have succeeded!\n");  // DEBUGGING
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


bool unmap_file(mapMem_ptr memStruct_ptr, bool syncMem)
{
	// LOCAL VARIABLES
	bool retVal = false;
	int errNum = 0;  // Store errno here
	errno = 0;
	
	// INPUT VALIDATION
	if (NULL != memStruct_ptr)
	{
		if (NULL != memStruct_ptr->fileMem_ptr && memStruct_ptr->memSize > 0)
		{
			// 1. Sync memory with the file IAW syncMem
			// if (true == syncMem)
			if (true == syncMem && false == memStruct_ptr->readOnly)
			{
				if (msync(memStruct_ptr->fileMem_ptr, memStruct_ptr->memSize, MS_INVALIDATE | MS_SYNC))
				{
					errNum = errno;
					HARKLE_ERROR(Map_Memory, unmap_file, msync failed);
					// fprintf(stderr, "unmap_file() - unable to msync mem to file\n");
					HARKLE_ERRNO(Map_Memory, msync, errNum);
				}
				else
				{
					// fprintf(stdout, "unmap_file() appears to have successfully msync()'d mem to file!\n");
				}
			}
			
			// 2. Unmap mem
			if (munmap(memStruct_ptr->fileMem_ptr, memStruct_ptr->memSize))
			{
				// fprintf(stderr, "memStruct_ptr->fileMem_ptr:\t%p\nmemStruct_ptr->memSize:\t%lu\n", memStruct_ptr->fileMem_ptr, memStruct_ptr->memSize);  // DEBUGGING
				errNum = errno;
				HARKLE_ERROR(Map_Memory, unmap_file, munmap failed);
				// fprintf(stderr, "unmap_file() - munmap() failed\n");
				HARKLE_ERRNO(Map_Memory, munmap, errNum);
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



