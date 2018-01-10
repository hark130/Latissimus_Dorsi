#include "Elf_Manipulation.h"
#include "Map_Memory.h"
#include <stdbool.h>	        // bool, true, false
#include <stdio.h>              // fprintf

int main(int argc, char *argv[])
{
    // LOCAL VARIABLES
    int retVal = -999;  // Change to 0 if map_file() succeeds and go from there
    mapMem_ptr elfBinary = NULL;
    mapMem_ptr codeCave = NULL;
    
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
    
    if (0 == retVal)
    {
        // 2. OPERATE ON ELF BINARY
        codeCave = find_code_cave(elfBinary);
        if (false == validate_struct(codeCave))
        {
            fprintf(stderr, "main() - validate_struct() doesn't agree with the return value from find_code_cave()!\n");
            retVal = -6;
        }
        else
        {
            fprintf(stdout, "Found a code cave!\nPointer:\t%p\nMem Size:\t%zu\n", codeCave->fileMem_ptr, codeCave->memSize);
            fprintf(stdout, "Offset:\t\t%p\n", (void*)(codeCave->fileMem_ptr - elfBinary->fileMem_ptr));
        }
    }
    
    // X. CLEAN UP
    if (NULL != elfBinary)
    {
        free_struct(&elfBinary);
    }

    // DONE
    return retVal;
}
