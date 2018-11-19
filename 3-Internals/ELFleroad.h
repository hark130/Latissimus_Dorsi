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
	OUTPUT
		On success, pointer to filename's file mapping
	NOTES
		Utilizes mmap()
 */
void *map_file(char *filename);


/*
	PURPOSE - Check a file for the ELF Magic Number
	INPUT
		filename - C String to a relative or absolute file
	OUTPUT
		true if is in ELF format
		Otherwise, false
 */
bool is_elf(char *filename);


#endif  // __ELFEROAD__
