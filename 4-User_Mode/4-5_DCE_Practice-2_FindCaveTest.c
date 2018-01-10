#include "Elf_Manipulation.h"
#include "Map_Memory.h"
#include <stdbool.h>	        // bool, true, false
#include <stdio.h>              // fprintf

typdef struct testFindCaveStruct
{
    char* inputMem;
    size_t inputSize;
    int inputMemType;
    char* expected_ptr;
    size_t expectedSize;
    char* actual_ptr;
    size_t actualSize;
} testStruct;

#define DEFAULT_SIZE ((int)-42)

int main(void)
{
    // LOCAL VARIABLES
    int retVal = 0;
    mapMem testInput = { NULL, 0, 0 };
    mapMem_ptr testOutput = NULL;
    int i = 0;

    // Test input
    // 1. Nothing, expect NULL
    char array1[] = { 'a', 'b', 'c', 'd', 'e' };
    size_t array1size = sizeof(array1) / sizeof(char);
    // 2. In the middle
    char array2[] = { 'a', 'b', 'c', 0x0, 0x0, 'd', 'e' };
    size_t array2size = sizeof(array2) / sizeof(char);
    // 3. In the front
    char array3[] = { 0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd', 'e' };
    size_t array3size = sizeof(array3) / sizeof(char);
    // 4. In the back
    char array4[] = { 'a', 'b', 'c', 'd', 'e', 0x0, 0x0, 0x0, 0x0 };
    size_t array4size = sizeof(array4) / sizeof(char);
    // 5. Two competing (2nd one wins)
    char array5[] = { 'a', 'b', 'c', 'd', 'e', 0x0, 0x0, 0x0, 0x0, 'f', 'g', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 'h' };
    size_t array5size = sizeof(array5) / sizeof(char);
    // 6. Two competing (2nd one wins)
    char array6[] = { 'a', 'b', 'c', 'd', 'e', 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 'f', 'g', 0x0, 0x0, 0x0, 0x0, 'h' };
    size_t array6size = sizeof(array6) / sizeof(char);
    // 7. Two competing at bounds (1st wins by one)
    char array7[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 0x0, 0x0, 0x0, 0x0 };
    size_t array7size = sizeof(array7) / sizeof(char);
    // 8. Two competing at bounds (1st wins by one)
    char array8[] = { 0x0, 0x0, 0x0, 0x0, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 0x0, 0x0, 0x0, 0x0, 0x0 };
    size_t array8size = sizeof(array8) / sizeof(char);
    // 9. Equal
    char array9[] = { 'a', 'b', 'c', 'd', 'e', 0x0, 0x0, 0x0, 0x0, 'f', 'g', 0x0, 0x0, 0x0, 0x0, 'h' };
    size_t array9size = sizeof(array9) / sizeof(char);

    // Array of test input
    testStruct normalTests[] = { \
        { array1, array1size, MM_TYPE_HEAP, NULL, 0, NULL, DEFAULT_SIZE }, \
        { array2, array2size, MM_TYPE_HEAP, array2 + 3, 0, NULL, DEFAULT_SIZE }, \
        { array3, array3size, MM_TYPE_HEAP, array3, 0, NULL, DEFAULT_SIZE }, \
        { array4, array4size, MM_TYPE_HEAP, array4 + 5, 0, NULL, DEFAULT_SIZE }, \
        { array5, array5size, MM_TYPE_HEAP, array5 + 11, 0, NULL, DEFAULT_SIZE }, \
        { array6, array6size, MM_TYPE_HEAP, array6 + 5, 0, NULL, DEFAULT_SIZE }, \
        { array7, array7size, MM_TYPE_HEAP, array7, 0, NULL, DEFAULT_SIZE }, \
        { array8, array8size, MM_TYPE_HEAP, array8 + 11, 0, NULL, DEFAULT_SIZE }, \
        { array9, array9size, MM_TYPE_HEAP, array9 + 5, 0, NULL, DEFAULT_SIZE }, \
    };

    // RUN THE TESTS
    for (i = 0; i < sizeof(normalTests) / sizeof(testStruct); i++)
    {
        // Output
        fprintf(stdout, "TEST #%d:\n", i + 1);

        // Setup input
        testInput.fileMem_ptr = (*(normalTests + i)).inputMem;
        testInput.memSize = (*(normalTests + i)).inputSize;
        testInput.memType = (*(normalTests + i)).inputMemType;

        // Function call
        testOutput = find_code_cave(&testInput);

        // Save return
        if (NULL != testOutput)
        {
            (*(normalTests + i)).actual_ptr = testOutput->fileMem_ptr;
            (*(normalTests + i)).actualSize = testOutput->memSize;
        }

        // Validate return values
        fprintf(stdout, "\tPointer:\t");
        if ((*(normalTests + i)).expected_ptr == (*(normalTests + i)).actual_ptr)
        {
            fprintf(stdout, "Pass\n");
        }
        else
        {
            fprintf(stdout, "FAIL... expected 0x%p, received 0x%p\n", (*(normalTests + i)).expected_ptr, (*(normalTests + i)).actual_ptr);
        }
        fprintf(stdout, "\tSize:\t\t");
        if ((*(normalTests + i)).expectedSize == (*(normalTests + i)).actualSize)
        {
            fprintf(stdout, "Pass\n");
        }
        else
        {
            fprintf(stdout, "FAIL... expected %d, received %d\n", (*(normalTests + i)).expectedSize, (*(normalTests + i)).actualSize);
        }

        // Free the return value
        free_struct(&testOutput);
    }


    // DONE
    return retVal;
}

/*
typdef struct testFindCaveStruct
{
    char* inputMem;
    size_t inputSize;
    int inputMemType;
    char* expected_ptr;
    size_t expectedSize;
    char* actual_ptr;
    size_t actualSize;
} testStruct;
 */