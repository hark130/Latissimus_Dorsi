#include "Fileroad.h"
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fscanf, getchar
#include <unistd.h>		// read, stream macros

#ifndef FROAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define FROAD_MAX_TRIES 3
#endif  // FROAD_MAX_TRIES

#ifndef FROAD_SML_BUFF_SIZE
// MACRO to size small static arrays
#define FROAD_SML_BUFF_SIZE 32
#endif  // FROAD_SML_BUFF_SIZE

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION PROTOTYPES START ///////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION PROTOTYPES STOP ////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


void clear_stdin(void)
{
	char read = 0;
	while (EOF != read && '\n' != read)
	{
		read = getchar();
//		putchar(read);  // DEBUGGING
	}

	return;
}


int read_a_num(void)
{
	// LOCAL VARIABLES
	int retVal = 0;

	// READ
	fscanf(stdin, "%d", &retVal);

	// CLEAR STDIN
	clear_stdin();

	// DONE
	return retVal;
}


char* buff_a_num(void)
{
	// LOCAL VARIABLES
	char* retVal;
	char localBuff[FROAD_SML_BUFF_SIZE + 1] = { 0 };
	char* temp_ptr = NULL;  // Iterating variable to walk the buffer
	bool success = true;  // If anything fails, this becomes false
	ssize_t numBytesRead = 0;  // Return value from read()

	// READ
	numBytesRead = read(STDIN_FILENO, localBuff, FROAD_SML_BUFF_SIZE);

	if (numBytesRead > FROAD_SML_BUFF_SIZE)
	{
		// This should never happen
        // fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - How did read get too many bytes?!\n");
        HARKLE_ERROR(Fileroad, buff_a_num, How did read get too many bytes);
        success = false;
	}
	else if (numBytesRead <= 0)
	{
		// This should never happen
        // fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - 0 bytes read?!\n");
        HARKLE_ERROR(Fileroad, buff_a_num, 0 bytes read?);
        success = false;
	}
	else  // "Just write." -Goldilocks(?)
	{
		// VALIDATE READ
		temp_ptr = localBuff;

		// Look for non-numbers
		while (*temp_ptr != '\0' && success == true)
		{
			if (*temp_ptr < 48 || *temp_ptr > 57)
			{
				success = false;
			}
			else
			{
				temp_ptr++;
			}
		}

		// Copy it onto the heap
		if (success == true)
		{
			retVal = copy_a_string(localBuff);

			if (!retVal)
			{
				// fprintf(stderr, "<<<ERROR>>> - Fileroad - buff_a_num() - copy_a_string failed!\n");
        		HARKLE_ERROR(Fileroad, buff_a_num, copy_a_string failed);
				success = false;
			}
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////
