#include "Map_Memory.h"
#include <stdlib.h>
#include <string.h>

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
	// 1. Create mapMem_ptr
	// 2. Get file descriptor for filename
	// 3. Map the file descriptor into memory
	// 4. Close the file descriptor (mapping into memory increments the file's reference count)
	// 5. Populate mapMem_ptr
	// 6. Return
	/*
		#include <sys/mman.h>

		void * mmap (void *addr,
					 size_t len,
					 int prot,
					 int flags,
					 int fd,
					 off_t offset);
	 */
	// Protection: PROT_READ | PROT_WRITE | PROT_EXEC
	// Flags: MAP_SHARED - Writing into the mapping is equivalent to writing to the file
}


/*
	Purpose - Unmap a file's contents from memory
	Input - mappedMemory pointer
	Output - None
 */
void unmap_file(mapMem_ptr memStruct_ptr)
{
	// 1. Sync memory with the file
	/*
		#include <sys/mman.h>

		int msync (void *addr, size_t len, int flags);
		Flags: MS_INVALIDATE | MS_SYNC
	 */
	
	
	// DONE
	return;
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
			// Clear the pointer variables
			tempBuff = NULL;
			tempStruct_ptr->fileMem_ptr = NULL;
			// Clear memSize
			tempSize = 0;
			tempStruct_ptr->memSize = 0;

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

/*
	Refs:
		https://www.safaribooksonline.com/library/view/linux-system-programming/0596009585/ch04s03.html
 */
