/*
	This header will be a collection of basic functions to parse ELF-formatted files.  It is
		similar in function to Elf_Manipulation.h but will not be utilizing the Map_Memory.h
		helper functions/structs.  Here, we're doing it live!
 */

#ifndef __ELFEROAD__
#define __ELFEROAD__

#include <elf.h>
#include <stdbool.h>							// bool, true, false


/*
	PURPOSE - Map a file into read-only memory
	INPUT
		filename - C String to a relative or absolute file
		memSize_ptr [out] - Size of the mapped memory
	OUTPUT
		On success, pointer to filename's file mapping
	NOTES
		Utilizes mmap()
 */
void *map_file(char *filename, size_t *memSize_ptr);


/*
	PURPOSE - Check a file for the ELF Magic Number
	INPUT
		fileCont - Pointer to a read-only file mapping
	OUTPUT
		true if is in ELF format
		Otherwise, false
 */
bool is_elf(void *fileCont);


/*
	PURPOSE - Determine if the mapped file is a 32-bit ELF or 64-bit ELF
	INPUT
		fileCont - Pointer to a read-only file mapping
	OUTPUT
		On success:
			ELFCLASSNONE if fileCont is not an ELF file
			ELFCLASS32 if fileCont is a 32-bit ELF file
			ELFCLASS64 if fileCont is a 64-bit ELF file
		On failure, -1
 */
int determine_elf_class(void *fileCont);


/*
	PURPOSE - Unmap a file
	INPUT
		oldMem_ptr - Pointer the variable holding the old pointer
	OUTPUT
		On success, true
		On failure, false
 */
bool unmap_file(void **oldMem_ptr);


#endif  // __ELFEROAD__
