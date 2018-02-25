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
        -1 on error
        -999 on 'horrible' error
 */
int determine_elf_class(mapMem_ptr elfFile);


/*
	Purpose - Allocate and populate a Mapped_Memory_Elf64 struct with ELF details
	Input - mappedMemory struct pointer to an ELF file
	Output - Dynamically allocated Mapped_Memory_Elf64 pointer
	Notes
		Calls validate_struct() to validate elf64File
		Calls is_elf() to verify format
		Calls determine_elf_class() to verify this is the right function
 */
mapElf64_ptr populate_mapElf64_struct(mapMem_ptr elf64File);


/*
	Purpose - Determine the ELF file's base virtual address from the first
		LOAD program header
	Input - Mapped_Memory_Elf64 struct pointer to an ELF file's details
	Output - Value of the base virtual address for elf64File
 */
Elf64_Addr get_elf64_base_address(mapElf64_ptr elf64File);


/*
	Purpose - Find the program header entry associated with a given address
	Input
		elf64File - Mapped_Memory_Elf64 pointer of the 'haystack' ELF
		addr - Address somewhere inside the 'haystack' ELF
	Output
		On succeess, pointer to the program header table entry responsible for this address
		On failure, NULL
	Notes
		A valid use case of this function is that addr may not be contained within the program headers
			so NULL does not mean "error"
 */
Elf64_Phdr* find_this_prgm_hdr_64addr(mapElf64_ptr elf64File, Elf64_Addr addr);


/*
	Purpose - Find the section header entry associated with a given address
	Input
		elf64File - Mapped_Memory_Elf64 pointer of the 'haystack' ELF
		addr - Address somewhere inside the 'haystack' ELF
	Output
		On succeess, pointer to the section header table entry responsible for this address
		On failure, NULL
	Notes
		A valid use case of this function is that addr may not be contained within the section headers
			so NULL does not mean "error"
 */
Elf64_Shdr* find_this_sect_hdr_64addr(mapElf64_ptr elf64File, Elf64_Addr addr);


/*
	Purpose - Search an ELF binary for the largest section of 'nothing'
	Input - elfBinary - struct* mappedMemory
	Output - struct* mappedMemory which holds the address and size of the 
		largest section of 'nothing'
 */
mapMem_ptr find_code_cave(mapMem_ptr elfBinary);


#endif  // __ELF_MANIPULATION__
