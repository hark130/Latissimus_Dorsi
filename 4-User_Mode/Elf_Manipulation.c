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

