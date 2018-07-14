#ifndef __MAP_MEMORY__
#define __MAP_MEMORY__

#include <stdbool.h>		// bool, true, false
#include <stdlib.h>			// size_t

#ifndef NULL
#define NULL ((void*)0)
#endif  // NULL

// MEMORY TYPE MACROS
#define MM_TYPE_HEAP ((int)1)	// Anonymous memory
#define MM_TYPE_MMAP ((int)2)	// File mmap'd memory
#define MM_TYPE_CAVE ((int)3)	// code cave... mem not owned by this struct

typedef struct mappedMemory 
{
	char* fileMem_ptr;
	size_t memSize;
	int memType;
	bool readOnly;  // true if mapped with O_RDONLY
} mapMem, *mapMem_ptr;
/*
	NOTE: Updates to the mappedMemory struct and/or memory type macros
		constitutes a change to the following functions:
	- free_struct()
	- validate_struct()
 */


/*
	Purpose - Dynamically allocate a struct pointer
	Input - None
	Output - Pointer to a mappedMemory struct on the heap
	Notes:
		mapMem_ptr must be free()'d by the calling function
		Calling function is responsible for setting the memType member
 */
mapMem_ptr create_mapMem_ptr(void);


/*
	Purpose - Map an anonymous mapping
	Input
		length - length of the mapping
		prot - protections (see: http://man7.org/linux/man-pages/man2/mmap.2.html)
		flags - flags (see: http://man7.org/linux/man-pages/man2/mmap.2.html)
	Output - Pointer to a mappedMemory struct on the heap
	Notes:
		mapMem_ptr must be free()'d by the calling function
		The MAP_ANONYMOUS will automatically be ORd in
 */
mapMem_ptr map_anon(size_t length, int prot, int flags);


/*
	Purpose - Map a file's contents to memory
	Input - Filename to map into memory
	Output - Pointer to a struct mappedMemory
	Notes:
		mapMem_ptr must be free()'d by the calling function
		mapMem_ptr->fileMem_ptr must be free()'d by the calling function
		This function is now is a 'wrapper' around 
			map_file_mode(filename, O_RDWR) for the sake of backwards
			compatibility
 */
mapMem_ptr map_file(const char* filename);


/*
	Purpose - Map a file's contents to memory
	Input
		filename - Filename to map into memory
		flags - Bitwise OR of access modes, zero or more file creation flags 
			and file status flags 
			(see: http://man7.org/linux/man-pages/man2/open.2.html)
	Output - Pointer to a struct mappedMemory
	Notes:
		mapMem_ptr must be free()'d by the calling function
		mapMem_ptr->fileMem_ptr must be free()'d by the calling function
		The argument flags must include one of the following access modes:
       		O_RDONLY, O_WRONLY, or O_RDWR.
 */
mapMem_ptr map_file_mode(const char* filename, int flags);


/*
	Purpose - Unmap a file's contents from memory
	Input
		memStruct_ptr - mappedMemory pointer
		syncMem - if true, msync to file (unless memStruct_ptr->readOnly is true)
	Output - true on success, otherwise false
	Notes:
		Calling function is responsible for free()'ing memStruct_ptr on success
 */
bool unmap_file(mapMem_ptr memStruct_ptr, bool syncMem);


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


/*
	Purpose - Validate the contents of a mappedMemory struct pointer
	Input - Pointer to a mappedMemory struct
	Output - true if valid, otherwise false
	Notes:
		Can not validate memType against the type of memory allocated
		Merely checks that memType is valid
 */
bool validate_struct(mapMem_ptr checkThis_ptr);


#endif  // __MAP_MEMORY__
