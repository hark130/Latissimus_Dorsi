#ifndef __ELF_MANIPULATION__
#define __ELF_MANIPULATION__

#include <elf.h>
#include "Map_Memory.h"
#include <stdbool.h>		// bool, true, false


typedef struct Mapped_Memory_Elf32
{
	mapMem_ptr binary_ptr;
	Elf32_Ehdr* binaryEhdr_ptr;
	Elf32_Phdr* binaryPhdr_ptr;
	Elf32_Shdr* binaryShdr_ptr;
} mapElf32, *mapElf32_ptr;


typedef struct Mapped_Memory_Elf64
{
	mapMem_ptr binary_ptr;
	Elf64_Ehdr* binaryEhdr_ptr;
	Elf64_Phdr* binaryPhdr_ptr;
	Elf64_Shdr* binaryShdr_ptr;
} mapElf64, *mapElf64_ptr;


/*
	Purpose - Check an mmap()'d file for the ELF Magic Number
	Input - file - struct* mappedMemory
	Output - true if in ELF, otherwise false
 */
bool is_elf(mapMem_ptr file);


/*
	Purpose - Determine if a mappedMemory file is 32-bit ELF or 64-bit ELF
	Input - mappedMemory struct pointer
	Output
		ELFCLASSNONE if mappedMemory is not an ELF file
		ELFCLASS32 if mappedMemory is a 32-bit ELF file
		ELFCLASS64 if mappedMemory is a 64-bit ELF file
 */
int determine_elf_class(mapMem_ptr elfFile);


/*
	Purpose - Search an ELF binary for the largest section of 'nothing'
	Input - elfBinary - struct* mappedMemory
	Output - struct* mappedMemory which holds the address and size of the 
		largest section of 'nothing'
 */
mapMem_ptr find_code_cave(mapMem_ptr elfBinary);


#endif  // __ELF_MANIPULATION__
