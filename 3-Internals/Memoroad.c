#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fprintf
#include <stdlib.h>		// calloc
#include <string.h>		// memset, memcpy

#ifndef MEMROAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define MEMROAD_MAX_TRIES 3
#endif  // MEMROAD_MAX_TRIES

#ifndef MEMSET_DEFAULT
#define MEMSET_DEFAULT 0x0
#endif  // MEMSET_DEFAULT

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


char* copy_a_string(const char* char_ptr)
{
    // LOCAL VARIABLES
    int numTries = 0;  // Max number to calloc attempts
    char* retVal = NULL;  // Allocated char array
    char* temp_ptr = NULL;  // Holds return value from memcpy
    size_t charLen = 0;  // Length of char_ptr
    
    // INPUT VALIDATION
    if (char_ptr)
    {
        if (*char_ptr)
        {    
            // 1. Size the char_ptr
            charLen = strlen(char_ptr);
            
            if (charLen > 0)
            {            
                // 2. Allocate a properly-sized array
                while (retVal == NULL && numTries < MEMROAD_MAX_TRIES)
                {
                    retVal = (char*)calloc(charLen + 1, sizeof(char));
                    numTries++;
                }
                
                if (retVal)
                {                
                    // 3. Copy char_ptr into that array
                    temp_ptr = (char*)memcpy((void*)retVal, (void*)char_ptr, charLen * sizeof(char));
                    
                    if (temp_ptr != retVal)
                    {
                        // fprintf(stderr, "<<<ERROR>>> - Fileroad - copy_a_string() - memcpy failed!\n");
        				HARKLE_ERROR(Fileroad, copy_a_string, memcpy failed);
                        // Free retVal
                        free(retVal);
                        // NULL retVal
                        retVal = NULL;
                    }
                }
                else
                {
                    // fprintf(stderr, "<<<ERROR>>> - Fileroad - copy_a_string() - calloc failed!\n");
        			HARKLE_ERROR(Fileroad, copy_a_string, calloc failed);
                }                
            }
        }
    }
    
    // DONE
    return retVal;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a char array
	Output - On success, true. Otherwise, false.
	Notes:
		Will attempt to memset, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray);
 */
bool release_a_string(char** charPtr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char* char_ptr = NULL;
	size_t stringLen = 0;  // Attempt to get the string length

	if (charPtr_ptr)
	{
		if (*charPtr_ptr)
		{
			retVal = release_a_string_len(charPtr_ptr, strlen(*charPtr_ptr));
		}
		else
		{
			HARKLE_ERROR(Fileroad, release_a_string, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Fileroad, release_a_string, NULL pointer);
		retVal = false;
	}

	// DONE
	return retVal;
}


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a heap-allocated char array
		buffSize - The size, in memory, of *charPtr_ptr
	Output - On success, true. Otherwise, false.
	Notes:
		Will memset buffSize byptes, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray, buffSize);
 */
bool release_a_string_len(char** charPtr_ptr, size_t buffSize)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Default... prove it wrong
	char* char_ptr = NULL;  // Easier to deal with it this way
	char* temp_ptr = NULL;  // Return value from string.h function calls

	// INPUT VALIDATION
	if (!charPtr_ptr)
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, NULL pointer);
		retVal = false;
	}
	else if (!(*charPtr_ptr))
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, NULL pointer);
		retVal = false;
	}
	else if (buffSize < 0)
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, Invalid buffer size);
		retVal = false;
	}

	if (retVal == true)
	{
		// RELEASE THE STRING
		char_ptr = *charPtr_ptr;

		if (buffSize > 0)
		{
			// 1. memset
			temp_ptr = (void*)memset((void*)char_ptr, MEMSET_DEFAULT, buffSize);

			if (temp_ptr != char_ptr)
			{
				HARKLE_ERROR(Fileroad, release_a_string_len, memset failed);
			}
		}

		// 2. free
		free(char_ptr);

		// 3. NULL
		char_ptr = NULL;
		*charPtr_ptr = NULL;
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////
