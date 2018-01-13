#include "Elf_Manipulation.h"
#include "Map_Memory.h"
#include <stdbool.h>	        // bool, true, false
#include <stdio.h>              // fprintf
#include <dlfcn.h>


int main(int argc, char *argv[])
{
    // LOCAL VARIABLES
    int retVal = -999;  // Change to 0 if map_file() succeeds and go from there
    mapMem_ptr elfBinary = NULL;
    mapMem_ptr codeCave = NULL;
    mapElf64_ptr elfStruct_ptr = NULL;
    void(*func_ptr)(void) = NULL;
    Elf64_Addr baseVirtAddr = 0;  // Holds the value of the base virtual address
    Elf64_Phdr* codeCavePrgmHdr = NULL;  // Holds a pointer to the Program Header entry which contains the code cave
    Elf64_Addr codeCaveOffset = 0;  // Calculates the relative offset of the Code Cave
    
    // INPUT VALIDATION
    if (2 > argc)
    {
        fprintf(stderr, "main() - Not enough arguments!\n");
    }
    else if (2 < argc)
    {
        fprintf(stderr, "main() - Too many arguments!\n");
    }
    else
    {
        // 1. MAP FILE INTO MEMORY
        // 1.1. Map the file
        elfBinary = map_file(argv[1]);
        // 1.2. Verify mapping succeeded
        // 1.2.1. Verify the struct pointer was allocated
        if (NULL == elfBinary)
        {
            fprintf(stderr, "main() - map_file() failed to allocate a mappedMemory struct!\n");
            retVal = -1;
        }
        else
        {
            retVal = 0;  // Going good so far

            // 1.2.2. Verify the struct's memSize is present
            if (0 >= elfBinary->memSize)
            {
                fprintf(stderr, "main() - map_file() appears to have improperly calculated the size of '%s' as %zu!\n", argv[1], elfBinary->memSize);
                retVal = -2;
            }
            
            // 1.2.3. Verify the struct's memType is correct
            if (MM_TYPE_MMAP != elfBinary->memType)
            {
                fprintf(stderr, "main() - map_file() appears to have improperly set the memory type of '%s' as %d!\n", argv[1], elfBinary->memType);
                retVal = -3;
            }
            
            // 1.2.4. Verify the struct's fileMem pointer is present
            if (NULL == elfBinary->fileMem_ptr)
            {
                fprintf(stderr, "main() - map_file() failed to map '%s'!\n", argv[1]);
                retVal = -4;
            }
            else
            {
                // 1.3. Verify argv[1] is in the ELF format
                if (false == is_elf(elfBinary))
                {
                    fprintf(stderr, "main() - is_elf() reports that '%s' does not appear to be in the ELF!\n", argv[1]);
                    retVal = -5;
                }
            }
        }        
    }
    

    // 2. OPERATE ON ELF BINARY
    // 2.1. Map elfBinary to Mapped_Memory_Elf64 struct pointer
    if (0 == retVal)
    {
        if (ELFCLASS64 != determine_elf_class(elfBinary))
        {
            fprintf(stdout, "This ELF binary class is not supported.\n");
        }
        else
        {
            elfStruct_ptr = populate_mapElf64_struct(elfBinary);
            if (NULL == elfStruct_ptr)
            {
                fprintf(stdout, "main() - populate_mapElf64_struct() has returned NULL!\n");
                retVal = -6;
            }
        }
    }

    // 2.2. Determine entry point
    if (0 == retVal)
    {
        // DEBUGGING
        // Verify address
        fprintf(stdout, "ELF Header:\t\t%p\nProgram Header:\t\t%p\nSection Header:\t\t%p\n", \
                (void*)elfStruct_ptr->binaryEhdr_ptr, \
                (void*)elfStruct_ptr->binaryPhdr_ptr, \
                (void*)elfStruct_ptr->binaryShdr_ptr);  // DEBUGGING
        if (elfStruct_ptr->binaryEhdr_ptr->e_entry)
        {
            fprintf(stdout, "Entry Point Offset:\t%p\n", (void*)elfStruct_ptr->binaryEhdr_ptr->e_entry);  // DEBUGGING
            fprintf(stdout, "Absolute Entry Point:\t%p\n", (void*)(elfStruct_ptr->binaryEhdr_ptr + \
                    elfStruct_ptr->binaryEhdr_ptr->e_entry));  // DEBUGGING
        }
        else
        {
            fprintf(stdout, "No Entry Point\n");
        }
    }

    // 2.3. Determine base virtual address
    if (0 == retVal)
    {
        if (0 < elfStruct_ptr->binaryEhdr_ptr->e_phnum)
        {
            baseVirtAddr = get_elf64_base_address(elfStruct_ptr);
            if (0 == baseVirtAddr)
            {
                fprintf(stderr, "main() - get_elf64_base_address() returned NULL!\n");
                retVal = -7;
            }
            else
            {
                fprintf(stdout, "Base virtual address:\t%p\n", (void*)baseVirtAddr);
            }
        }
        else
        {
            fprintf(stdout, "No Program Headers\n");
        }
    }

    // 2.4. Find a Code Cave
    if (0 == retVal)
    {
        codeCave = find_code_cave(elfBinary);
        if (false == validate_struct(codeCave))
        {
            fprintf(stderr, "main() - validate_struct() doesn't agree with the return value from find_code_cave()!\n");
            retVal = -8;
        }
        else
        {
            codeCaveOffset = (Elf64_Addr)(codeCave->fileMem_ptr - elfBinary->fileMem_ptr);
            fprintf(stdout, "\nFound a code cave!\nPointer:\t%p\nMem Size:\t%zu\n", codeCave->fileMem_ptr, codeCave->memSize);
            fprintf(stdout, "Offset:\t\t%p\n", (void*)codeCaveOffset);
        }
    }
    
    // 2.5. Where is that Code Cave located
    if (0 == retVal)
    {
        // Look in the ELF Header

        
        // Look in the Program Header entries
        codeCavePrgmHdr = find_this_prgm_hdr_64addr(elfStruct_ptr, codeCaveOffset);
        // codeCavePrgmHdr = find_this_prgm_hdr_64addr(elfStruct_ptr, (Elf64_Addr) codeCave->fileMem_ptr);
        if (NULL == codeCavePrgmHdr)
        {
            fprintf(stdout, "The code cave at %p does not appear to be in a Program Header.\n", (void*)codeCave->fileMem_ptr);    
        }
        else
        {
            fprintf(stdout, "Found code cave address %p inside the Program Header entry at %p.\n", (void*)codeCave->fileMem_ptr, (void*)codeCavePrgmHdr);
        }

        // Look in the Section Header entries

       
        // Verify we found something
        if (NULL == codeCavePrgmHdr)  // Add ELF and Section Header return values here later
        {
            retVal = -9;
        }
    }

    // 2.6. Set the Code Cave section to execute
    if (0 == retVal)
    {
        // NOTE: Maybe unnecessary given that code caves could be in sections with flags to set to "execute"

        // ELF Header

        // Program Header
        if (NULL != codeCavePrgmHdr)
        {
            codeCavePrgmHdr->p_flags |= PF_X;  // Set the exeucte flag
        }

        // Section Header
    }
    
    // X. CLEAN UP
    if (NULL != elfBinary)
    {
        free_struct(&elfBinary);
    }
    if (NULL != elfStruct_ptr)
    {
        free(elfStruct_ptr);
        elfStruct_ptr = NULL;
    }

    // DONE
    return retVal;
}
