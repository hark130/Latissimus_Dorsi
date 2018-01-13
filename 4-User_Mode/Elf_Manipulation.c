#include <elf.h>
#include "Elf_Manipulation.h"
#include <inttypes.h>       // uint format specifiers
#include "Map_Memory.h"
#include <stdbool.h>		// bool, true, false
#include <stdio.h>          // fprintf
#include <string.h>         // strstr


#ifndef MAX_TRIES
// MACRO to limit repeated allocation attempts
#define MAX_TRIES 3
#endif // MAX_TRIES

bool is_elf(mapMem_ptr file)
{
    // LOCAL VARIABLES
    bool retVal = false;
    char* file_ptr = NULL;
    char* return_ptr = NULL;
    char magicNum[] = { 0x7F, 'E', 'L', 'F', 0x0 };
    
    // INPUT VALIDATION
    if (NULL != file)
    {
        if (NULL != file->fileMem_ptr)
        {
            // FIND ELF MAGIC NUMBER
            return_ptr = strstr(file->fileMem_ptr, magicNum);
            
            if (file->fileMem_ptr == return_ptr)
            {
                retVal = true;
            }
            else if (NULL != return_ptr)
            {
                retVal = true;
                fprintf(stdout, "Found the ELF magic number at 0x%p but the mapped file begins at 0x%p", return_ptr, file->fileMem_ptr);  // DEBUGGING
            }
        }
    }
    
    // DONE
    return retVal;
}


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
int determine_elf_class(mapMem_ptr elfFile)
{
    // LOCAL VARIABLES
    int retVal = -999;
    char* magicNum = NULL;
    
    // INPUT VALIDATION
    // 0. Is the mapMem_ptr an actual pointer?
    if (NULL == elfFile)
    {
        retVal = -1;
    }    
    // 1. Is elfFile a valid mappedMemory struct
    else if (false == validate_struct(elfFile))
    {
        retVal = -1;
    }
    // 2. Is elfFile actually an ELF file
    else if (false == is_elf(elfFile))
    {
        retVal = ELFCLASSNONE;
    }    
    // 3. Is elfFile 32-bit or 64-bit?
    else
    {
        magicNum = elfFile->fileMem_ptr;
        
        if (NULL != magicNum)
        {
            // "The fifth byte identifies the architecture for this binary"
            retVal = (int)(*(magicNum + 4));
            // Validate what we found
            if (retVal != ELFCLASSNONE && \
                retVal != ELFCLASS32 && \
                retVal != ELFCLASS64)
            {
                fprintf(stderr, "determine_elf_class() - found an unsupported EI_CLASS value of %d!\n", retVal);
                retVal = -999;    
            }
        }
        else
        {
            retVal = -1;    
        }
    }
    
    // DONE
    return retVal;
}


/*
	Purpose - Allocate and populate a Mapped_Memory_Elf64 struct with ELF details
	Input - mappedMemory struct pointer to an ELF file
	Output - Dynamically allocated Mapped_Memory_Elf64 pointer
	Notes
		Calls validate_struct() to validate elf64File
		Calls is_elf() to verify format
		Calls determine_elf_class() to verify this is the right function
 */
mapElf64_ptr populate_mapElf64_struct(mapMem_ptr elf64File)
{
    // LOCAL VARIABLES
    mapElf64_ptr retVal = NULL;
	int numTries = 0;
    
	// INPUT VALIDATION
	// 1. Validate mappedMemory struct pointer
	if (false == validate_struct(elf64File))
	{
		fprintf(stderr, "populate_mapElf64_struct() - elf64File is an invalid mappedMemory struct pointer!\n");	
	}
	// 2. Verify elf64File is actually an ELF file
	else if (false == is_elf(elf64File))
	{
		fprintf(stderr, "populate_mapElf64_struct() - elf64File is, in fact, NOT and ELF file!\n");
	}
	// 3. Verify this ELF file is, in fact, 64-bit ELF
	else if (ELFCLASS64 != determine_elf_class(elf64File))
	{
		fprintf(stderr, "populate_mapElf64_struct() - elf64File is not a 64-bit ELF file!\n");
	}
	else
	{	
		// ALLOCATE A STRUCT
		while (NULL == retVal && numTries < MAX_TRIES)
		{
			retVal = (mapElf64_ptr)calloc(1, sizeof(mapElf64));
			numTries++;
		}
		
		if (NULL == retVal)
		{
			fprintf(stderr, "populate_mapElf64_struct() - allocation of Mapped_Memory_Elf64 struct has failed %d times!\n", numTries);
		}
		else
		{
			// MAP ELF FILE HEADERS
			// 1. mappedMemory struct pointer
			retVal->binary_ptr = elf64File;
			// 2. ELF Header pointer
			retVal->binaryEhdr_ptr = (Elf64_Ehdr*) retVal->binary_ptr->fileMem_ptr;
			// 3. Program Header pointer
			retVal->binaryPhdr_ptr = (Elf64_Phdr*) (retVal->binary_ptr->fileMem_ptr + retVal->binaryEhdr_ptr->e_phoff);
			// 4. Section Header pointer
			retVal->binaryShdr_ptr = (Elf64_Shdr*) (retVal->binary_ptr->fileMem_ptr + retVal->binaryEhdr_ptr->e_shoff);
		}		
	}
	
    // DONE
    return retVal;
}


/*
    Purpose - Determine the ELF file's base virtual address from the first
        LOAD program header
    Input - Mapped_Memory_Elf64 struct pointer to an ELF file's details
    Output - Value of the base virtual address for elf64File
 */
Elf64_Addr get_elf64_base_address(mapElf64_ptr elf64File)
{
    // LOCAL VARIABLES
    Elf64_Addr retVal = 0;  // Value of the ELF's base virtual address
    Elf64_Phdr* currPrgmHdr_ptr = NULL;  // Program Header pointer
    // Elf64_Half pHdrsize = 0;  // Size of each Program Header
    Elf64_Half currHdrNum = 0;  // Tracks the current header number
    Elf64_Half numPrgmHdrs = 0;  // Number of Program Headers

    // INPUT VALIDATION
    if (NULL == elf64File)
    {
        fprintf(stderr, "get_elf64_base_address() - elf64File is NULL\n");
    }
    else
    {
        // GET PROGRAM HEADER DETAILS
        // Program Header size
        // pHdrsize = elf64File->binaryEhdr_ptr->e_phentsize;
        // Number of Program Headers
        numPrgmHdrs = elf64File->binaryEhdr_ptr->e_phnum;
        // First Program Header
        currPrgmHdr_ptr = elf64File->binaryPhdr_ptr;

        // PARSE PROGRAM HEADERS
        for (currHdrNum = 1; currHdrNum <= numPrgmHdrs; currHdrNum++)
        {
            // fprintf(stdout, "Checking Program Header #%d\t", currHdrNum);  // DEBUGGING
            if (PT_LOAD == currPrgmHdr_ptr->p_type)
            {
                // fprintf(stdout, "PT_LOAD found!\n");  // DEBUGGING
                retVal = currPrgmHdr_ptr->p_vaddr;
                break;
            }
            else
            {
                // fprintf(stdout, "currPrgmHdr_ptr->p_type == %"PRIu32"\n", currPrgmHdr_ptr->p_type);  // DEBUGGING
                currPrgmHdr_ptr++;
            }
        }
    }

    // DONE
    return retVal;
}


/*
	Purpose - Find the program header entry associated with a given address
	Input
		elf64File - Mapped_Memory_Elf64 pointer of the 'haystack' ELF
		addr - Address somewhere inside the 'haystack' ELF
	Output
		On succeess, pointer to the program header table entry responsibile for this address
		On failure, NULL
	Notes
		A valid use case of this function is that addr may not be contained within the program headers
			so NULL does not mean "error"
 */
Elf64_Phdr* find_this_prgm_hdr_64addr(mapElf64_ptr elf64File, Elf64_Addr addr)
{
	// LOCAL VARIABLES
	Elf64_Phdr* retVal = NULL;
	Elf64_Phdr* currProgHdr = NULL;
	int progHdrNum = 0;
	
	// INPUT VALIDATION
	if (NULL != elf64File && 0 != addr)
	{
		currProgHdr = elf64File->binaryPhdr_ptr;
		progHdrNum = 1;
		
		while (NULL != currProgHdr && progHdrNum <= elf64File->binaryEhdr_ptr->e_phnum)
		{
			// DEBUGGING
			// fprintf(stdout, "Looking for:\t%p in Program Header #%d\n", (void*) addr, progHdrNum);
			// fprintf(stdout, "\tp_offset == %p\n", (void*) currProgHdr->p_offset);
			// fprintf(stdout, "\tp_vaddr == %p\n", (void*) currProgHdr->p_vaddr);
			// fprintf(stdout, "\tp_paddr == %p\n", (void*) currProgHdr->p_paddr);
			

			// If found it, store, and continue checking
			if (currProgHdr->p_offset < addr)  // IMPLEMENT THIS LATER
			{
                // Already found one
                if (NULL != retVal)
                {
                    if (currProgHdr->p_offset > retVal->p_offset)
                    {
                        retVal = currProgHdr;
                        // fprintf(stdout, "\tFOUND ONE!\nCod Cave Offset %p could be in Program Header Offset %p\n", \
                                (void*)addr, (void*)retVal->p_offset);  // DEBUGGING
                    }
                }
                // First one we found
                else
                {
                    retVal = currProgHdr;
                    // fprintf(stdout, "\tFOUND ONE!\nCod Cave Offset %p could be in Program Header Offset %p\n", \
                            (void*)addr, (void*)retVal->p_offset);  // DEBUGGING
                }
			}
			else
			{
				// fprintf(stdout, "\tDidn't find it here!\n");  // DEBUGGING
			}
            currProgHdr++;  // Next Program Header entry 
            progHdrNum++;  // Increment the number to match the entry
		}
        // fprintf(stdout, "\tFOUND IT!\nCod Cave Offset %p is DEFINITELY in Program Header Offset %p\n", (void*)addr, (void*)retVal->p_offset);  // DEBUGGING
	}
	
	// DONE
	return retVal;
}


/*
    Purpose - Search an ELF binary for the largest section of 'nothing'
    Input - elfBinary - struct* mappedMemory
    Output - struct* mappedMemory which holds the address and size of the 
        largest section of 'nothing'
 */
mapMem_ptr find_code_cave(mapMem_ptr elfBinary)
{
    // LOCAL VARIABLES
    mapMem_ptr retVal = NULL;
    char* tempAddr_ptr = NULL;  // Pointer to the beginning of the current section
    char* currChar_ptr = NULL;  // Pointer used to validate each char
    int byteCounter = 0;        // Counts the size of the 'nothing' segment

    // INPUT VALIDATION
    if (true == validate_struct(elfBinary))
    {
        // puts("MADE IT PAST validate_struct()");  // DEBUGGING
        // 1. Create the return struct
        retVal = create_mapMem_ptr();
        retVal->fileMem_ptr = elfBinary->fileMem_ptr;
        retVal->memSize = 0;
        retVal->memType = MM_TYPE_CAVE;
        currChar_ptr = elfBinary->fileMem_ptr;
        tempAddr_ptr = elfBinary->fileMem_ptr;

        // puts("MADE IT HERE!");  // DEBUGGING
        // 2. Start counting
        while (currChar_ptr < (elfBinary->fileMem_ptr + elfBinary->memSize))
        {
            // puts("ITERATING!");  // DEBUGGING
            // putchar(*currChar_ptr);  // DEBUGGING
            // Found empty char
            if (0x0 == *currChar_ptr)
            {
                // puts("MADE IT HERE!");  // DEBUGGING
                byteCounter++;
            }
            // End of empty chars
            else
            {
                // Larger than value stored in retVal
                if (byteCounter > retVal->memSize)
                {
                    // puts("FOUND ONE!");  // DEBUGGING
                    // Set struct to beginning of this section
                    retVal->fileMem_ptr = tempAddr_ptr;
                    // Set the size
                    retVal->memSize = byteCounter;
                    // fprintf(stdout, "Current winner pointer:\t%p\n", retVal->fileMem_ptr);  // DEBUGGING
                    // fprintf(stdout, "Current winner length:\t%zu\n", retVal->memSize);  // DEBUGGING
                }

                // Reset...
                // ...byteCounter
                byteCounter = 0;
                // ...tempAddr_ptr
                tempAddr_ptr = currChar_ptr + 1;
            }

            // Increment to next byte
            currChar_ptr++;
        }
        // Trailing "code cave"
        // Larger than value stored in retVal
        if (byteCounter > retVal->memSize)
        {
            // puts("FOUND ONE!");  // DEBUGGING
            // Set struct to beginning of this section
            retVal->fileMem_ptr = tempAddr_ptr;
            // Set the size
            retVal->memSize = byteCounter;
            // fprintf(stdout, "Current winner pointer:\t%p\n", retVal->fileMem_ptr);  // DEBUGGING
            // fprintf(stdout, "Current winner length:\t%zu\n", retVal->memSize);  // DEBUGGING
        }
    }

    // Never found one
    if (0 == retVal->memSize && retVal->fileMem_ptr == elfBinary->fileMem_ptr)
    {
        retVal->fileMem_ptr = NULL;
    }

    // DONE
    return retVal;
}

