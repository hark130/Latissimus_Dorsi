#include <fcntl.h>      // open() flags
#include "Fileroad.h"
#include "Memoroad.h"
#include <stdbool.h>	// bool, true, false
#include <stdio.h>		// fscanf, getchar
#include <stdlib.h>     // calloc
#include <string.h>     // strlen, strstr
#include <unistd.h>		// read, stream macros

#ifndef FROAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define FROAD_MAX_TRIES 3
#endif  // FROAD_MAX_TRIES

#ifndef FROAD_BUFF_SIZE
// MACRO to size static arrays
#define FROAD_BUFF_SIZE 1024
#endif  // FROAD_BUFF_SIZE

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
	bool success = true;  // If anything fails, this becomes false
	ssize_t numBytesRead = 0;  // Return value from read()
	int i = 0;  // Iterating variable used to check each char

	// READ
	numBytesRead = read(STDIN_FILENO, localBuff, FROAD_SML_BUFF_SIZE);
	// fprintf(stdout, "numBytesRead == %zu\n", numBytesRead);  // DEBUGGING
	// fprintf(stdout, "localBuff == %s\n", localBuff);  // DEBUGGING

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
		// REMOVE ANY NEWLINES
		// puts("Entering buff_a_num() removing newlines");  // DEBUGGING
		i = 0;  // Reset temp variable
		while (i < FROAD_SML_BUFF_SIZE && (*(localBuff + i)) != '\0' && success == true)
		{
			if ((*(localBuff + i)) == '\n')
			{
				(*(localBuff + i)) = '\0';
			}
			else if ((*(localBuff + i)) == '\r')
			{
				(*(localBuff + i)) = '\0';
			}
			else
			{
				i++;
			}
		}

		// puts("Entering buff_a_num() read");  // DEBUGGING
		// VALIDATE READ
		// Look for non-numbers
		i = 0;  // Reset temp variable
		while (i < FROAD_SML_BUFF_SIZE && (*(localBuff + i)) != '\0' && success == true)
		{
			// fprintf(stdout, "Looking at:\t%c (0x%X)\n", (*(localBuff + i)), (*(localBuff + i)));  // DEBUGGING
			if ((*(localBuff + i)) < 48 || (*(localBuff + i)) > 57)
			{
				success = false;
			}
			else
			{
				i++;
			}
		}

		// Copy it onto the heap
		if (success == true)
		{
			// puts("Entering buff_a_num() heap allocation");  // DEBUGGING
			// fprintf(stdout, "localBuff == %s\n", localBuff);  // DEBUGGING
			retVal = copy_a_string(localBuff);
			// fprintf(stdout, "retVal == %s\n", retVal);  // DEBUGGING

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
//////////////////////////// FILE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


char* read_a_file(char* fileName)
{
    // LOCAL VARIABLES
    char* retVal = NULL;
    int fileDesc = 0;  // Holds the file descriptor returned by open()
    bool success = true;  // If anything fails, set this to false
    char buff[FROAD_BUFF_SIZE + 1] = { 0 };  // Static array to read file contents
    ssize_t numBytesRead = 0;  // Return value from read()
    int numTries = 0;  // Keeps track of allocation attempts
    char* temp_ptr = NULL;  // Return values from string.h function calls

    // INPUT VALIDATION
    if (fileName)
    {
        if (*fileName)
        {
            // READ THE FILE
            // 1. Open the file
            if (success == true)
            {
                fileDesc = open(fileName, O_RDONLY);

                if (fileDesc < 0)
                {
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - open failed!\n");
        			HARKLE_ERROR(Harkleproc, read_a_file, open failed);
                    success = false;
                }
            }

            // 2. Read the file
            if (success == true)
            {
                numBytesRead = read(fileDesc, buff, FROAD_BUFF_SIZE);

                if (numBytesRead < 0)
                {
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - read failed!\n");
        			HARKLE_ERROR(Harkleproc, read_a_file, read failed);
                    success = false;
                }
                else if (numBytesRead == 0)
                {
                    // It's ok if 0 bytes were read.  Some cmdline files are empty.
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - 0 bytes read!\n");
                    // success = false;
                    temp_ptr = strcpy(buff, "<EMPTY>");

                    if (temp_ptr != buff)
                    {
                        // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - strcpy failed!\n");
        				HARKLE_ERROR(Harkleproc, read_a_file, strcpy failed);
                        success = false;
                    }
                    else
                    {
                        numBytesRead = strlen(buff);
                    }
                }
            }

            // 3. Allocate an array to store the file contents
            if (success == true && numBytesRead > 0)
            {
                while (!retVal && numTries < FROAD_MAX_TRIES)
                {
                    retVal = (char*)calloc(numBytesRead + 1, sizeof(char));
                    numTries++;
                }

                if (!retVal)
                {
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - calloc failed!\n");
        			HARKLE_ERROR(Harkleproc, read_a_file, calloc failed);
                    success = false;
                }
            }

            // 4. Read the file contents into the heap-allocated array
            if (success == true && retVal)
            {
                temp_ptr = strncpy(retVal, buff, numBytesRead);

                if (temp_ptr != retVal)
                {
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - strncpy failed!\n");
        			HARKLE_ERROR(Harkleproc, read_a_file, strncpy failed);
                    success = false;
                }
            }
        }
    }
    else
    {
        // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - NULL fileName!\n");
        HARKLE_ERROR(Harkleproc, read_a_file, NULL fileName);
        success = false;
    }

    // CLEAN UP
    if (success == false)
    {
        if (retVal)
        {
            // memset
            if (*retVal)
            {
                temp_ptr = memset(retVal, 0x0, strlen(retVal));

                if (temp_ptr != retVal)
                {
                    // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - memset failed!\n");
        			HARKLE_ERROR(Harkleproc, read_a_file, memset failed);
                }

                temp_ptr = NULL;
            }

            // free
            free(retVal);

            // NULL
            retVal = NULL;
        }
    }

    // Close the file descriptor regardless of the success status
    if (fileDesc > 2)
    {
        if (close(fileDesc) < 0)
        {
            // fprintf(stderr, "<<<ERROR>>> - Harkleproc - read_a_file() - close failed!\n");
        	HARKLE_ERROR(Harkleproc, read_a_file, close failed);
        }
    }

    // DONE
    return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FILE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS START //////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
////////////////////// LOCAL FUNCTION DEFINITIONS STOP ///////////////////////
//////////////////////////////////////////////////////////////////////////////
