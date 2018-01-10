#include "Elf_Manipulation.h"
#include "Map_Memory.h"
#include <stdbool.h>	        // bool, true, false
#include <stdio.h>              // fprintf

typedef struct testFindCaveStruct
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

    // Normal Test input
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
    testStruct unitTests[] = { \
        // Normal Tests
        { array1, array1size, MM_TYPE_HEAP, NULL,        0, NULL, DEFAULT_SIZE }, \
        { array2, array2size, MM_TYPE_HEAP, array2 + 3,  2, NULL, DEFAULT_SIZE }, \
        { array3, array3size, MM_TYPE_HEAP, array3,      4, NULL, DEFAULT_SIZE }, \
        { array4, array4size, MM_TYPE_HEAP, array4 + 5,  4, NULL, DEFAULT_SIZE }, \
        { array5, array5size, MM_TYPE_HEAP, array5 + 11, 8, NULL, DEFAULT_SIZE }, \
        { array6, array6size, MM_TYPE_HEAP, array6 + 5,  8, NULL, DEFAULT_SIZE }, \
        { array7, array7size, MM_TYPE_HEAP, array7,      5, NULL, DEFAULT_SIZE }, \
        { array8, array8size, MM_TYPE_HEAP, array8 + 11, 5, NULL, DEFAULT_SIZE }, \
        { array9, array9size, MM_TYPE_HEAP, array9 + 5,  4, NULL, DEFAULT_SIZE }, \
    };

    // RUN THE TESTS
    for (i = 0; i < sizeof(unitTests) / sizeof(testStruct); i++)
    {
        // Output
        fprintf(stdout, "TEST #%d:\n", i + 1);

        // Setup input
        testInput.fileMem_ptr = (*(unitTests + i)).inputMem;
        testInput.memSize = (*(unitTests + i)).inputSize;
        testInput.memType = (*(unitTests + i)).inputMemType;

        // Function call
        // puts("Pre-function call");  // DEBUGGING
        testOutput = find_code_cave(&testInput);
        // puts("Post-function call");  // DEBUGGING

        // Save return
        if (NULL != testOutput)
        {
            (*(unitTests + i)).actual_ptr = testOutput->fileMem_ptr;
            (*(unitTests + i)).actualSize = testOutput->memSize;
        }

        // Validate return values
        fprintf(stdout, "\tPointer:\t");
        if ((*(unitTests + i)).expected_ptr == (*(unitTests + i)).actual_ptr)
        {
            fprintf(stdout, "Pass\n");
        }
        else
        {
            fprintf(stdout, "FAIL... with array %p, expected %p, received %p\n", (*(unitTests + i)).inputMem, (*(unitTests + i)).expected_ptr, (*(unitTests + i)).actual_ptr);
        }
        fprintf(stdout, "\tSize:\t\t");
        if ((*(unitTests + i)).expectedSize == (*(unitTests + i)).actualSize)
        {
            fprintf(stdout, "Pass\n");
        }
        else
        {
            fprintf(stdout, "FAIL... expected %zu, received %zu\n", (*(unitTests + i)).expectedSize, (*(unitTests + i)).actualSize);
        }

        // Free the return value
        free_struct(&testOutput);
        // fprintf(stdout, "i == %d < %zu\n", i, sizeof(unitTests) / sizeof(unitTests));
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