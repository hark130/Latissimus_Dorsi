#ifndef __MAP_MEMORY__
#define __MAP_MEMORY__

#include <stdbool.h>		// bool, true, false
#include <stdlib.h>		// size_t

#ifndef NULL
#define NULL ((void*)0)
#endif  // NULL

typedef struct mappedMemory 
{
	char* fileMem_ptr;
	size_t memSize;
} mapMem, *mapMem_ptr;


/*
	Purpose - Dynamically allocate a struct pointer
	Input - None
	Output - Pointer to a mappedMemory struct on the heap
	Notes:
		mapMem_ptr must be free()'d by the calling function
 */
mapMem_ptr create_mapMem_ptr(void);


/*
	Purpose - Map a file's contents to memory
	Input - Filename to map into memory
	Output - Pointer to a struct mappedMemory
	Notes:
		mapMem_ptr must be free()'d by the calling function
		mapMem_ptr->fileMem_ptr must be free()'d by the calling function
 */
mapMem_ptr map_file(const char* filename);


/*
	Purpose - Unmap a file's contents from memory
	Input - mappedMemory pointer
	Output - None
 */
void unmap_file(mapMem_ptr memStruct_ptr);


/*
	Purpose - Free a mappedMemory struct on the heap
	Input - Pointer to a mappedMemory pointer to free
	Output - None
	Notes:
		Will memset(0x0), free, and NULL fileMem_ptr
		Will zeroize memSize
		Will memset(0x0), free, and NULL oldStruct_ptr
 */
void free_struct(mapMem_ptr* oldStruct_ptr);


#endif  // __MAP_MEMORY__
