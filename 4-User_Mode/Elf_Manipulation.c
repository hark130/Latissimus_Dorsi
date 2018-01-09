#include "Map_Memory.h"
#include <stdbool.h>		// bool, true, false
#include <stdio.h>          // fprintf
#include <string.h>         // strstr

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
