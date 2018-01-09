#ifndef __ELF_MANIPULATION__
#define __ELF_MANIPULATION__

#include "Map_Memory.h"
#include <stdbool.h>		// bool, true, false


/*
	Purpose - Check an mmap()'d file for the ELF Magic Number
	Input - file - struct* mappedMemory
	Output - true if in ELF, otherwise false
 */
bool is_elf(mapMem_ptr file);


/*
	Purpose - Search an ELF binary for the largest section of 'nothing'
	Input - elfBinary - struct* mappedMemory
	Output - struct* mappedMemory which holds the address and size of the 
		largest section of 'nothing'
 */
mapMem_ptr find_code_cave(mapMem_ptr elfBinary);


#endif  // __ELF_MANIPULATION__