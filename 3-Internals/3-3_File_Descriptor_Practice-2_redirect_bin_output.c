#include "Fileroad.h"
#include "Fileroad_Descriptors.h"
#include "Harklerror.h"
#include <stdio.h>
#include "Timeroad.h"


/*
	Purpose - Tailor-written to build an output/error filename for redirect_bin_output.exe
	Input
		timestamp - YYYYMMDD-HHMMSS in a nul-terminated string
		properName - Name of the binary in a nul-terminated string
		output - If true "output", otherwise "errors"
	Output
		On success, heap-allocated, nul-terminated string containing:
			YYYYMMDD-HHMMSS-properName-output.txt
			-or-
			YYYYMMDD-HHMMSS-properName-errors.txt
	Notes:
		It is the caller's responsibility to free the return value
		properName will be modified to remove certain characters
 */
char* build_filename(char* timestamp, char* properName, bool output);


int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	int i = 0;  // Iterating variable
	rBinDat_ptr silentBin = NULL;  // Return value from build_rBinDat_ptr()
	char* timestamp = NULL;  // Return value from build_timestamp() goes here
	char* temp_ptr = NULL;  // Iterating pointer variable
	int fullLogLen = 0;  // Calculated necessary length of the outputFile and errorsFile string lengths

	// INPUT VALIDATION
	// 1. Verify number of arguments
	if (argc < 2)
	{		
		fprintf(stderr, "\nToo few arguments!\nusage: redirect_bin_output.exe /path/to/bin -arg1 -arg2\n\n");
		success = false;
	}
	// 2. Verify each argument
	else
	{
		for (i = 1; i < argc; i++)
		{
			// 2.1. Verify it's not NULL
			if (!(argv[i]))
			{
				HARKLE_ERROR(redirect_bin_output, main, NULL pointer);
				success = false;	
			}
			// 2.2. Verify it's not empty
			else if (*(argv[i]) == '\0')
			{
				HARKLE_ERROR(redirect_bin_output, main, Empty argument);
				success = false;
			}
		}
		
		// fprintf(stdout, "silentBin->binName == %s\n", argv[1]);  // DEBUGGING
	}

	// 3. Verify it actually exists
	if (success == true)
	{
		success = os_path_exists(argv[1]);

		if (success == false)
		{
			fprintf(stderr, "\nCan not find binary file %s!\nusage: redirect_bin_output.exe /path/to/bin -arg1 -arg2\n\n", argv[1]);
		}
	}

	// ALLOCATE AND POPULATE THE STRUCT
	if (success == true)
	{
		// 1. Allocate and populate the struct
		silentBin = build_rBinDat_ptr(argv[1]);

		if (!silentBin)
		{
			HARKLE_ERROR(redirect_bin_output, main, Empty argument);
			success = false;
		}
		else
		{
			// 2. Get timestamp
			timestamp = build_timestamp();

			if (!timestamp)
			{
				HARKLE_ERROR(redirect_bin_output, main, build_timestamp failed);
				success = false;
			}
			else if (strlen(timestamp) != 15)
			{
				HARKLE_ERROR(redirect_bin_output, main, timestamp improperly formatted);
				success = false;
			}
			else
			{				
				fprintf(stdout, "Timestamp is %s\n", timestamp);  // DEBUGGING

				// 3. Build the output/errors filenames
				// 3.1. Determine necessary string length
				// YYYYMMDD-HHMMSS-strlen(binary)-output.txt
				// 1234567890123456 + strlen() + 12345678901
				fullLogLen = 15 + 1 + strlen(argv[1]) + 11;
				// 3.2. Allocate arrays for both filenames
				silentBin->outputFile = get_me_a_buffer(fullLogLen);
				silentBin->errorsFile = get_me_a_buffer(fullLogLen);

				if (!(silentBin->outputFile) || !(silentBin->errorsFile))
				{
					HARKLE_ERROR(redirect_bin_output, main, get_me_a_buffer failed);
					success = false;
				}
				else
				{
					// 3.3. Copy the timestamp in
					// 3.3.1. outputFile
					temp_ptr = strncmp(silentBin->outputFile, timestamp, 15);

					if (temp_ptr != silentBin->outputFile)
					{
						HARKLE_ERROR(redirect_bin_output, main, strncmp failed);
						success = false;
					}

					// 3.3.2. errorsFile
					temp_ptr = strncmp(silentBin->errorsFile, timestamp, 15);

					if (temp_ptr != silentBin->errorsFile)
					{
						HARKLE_ERROR(redirect_bin_output, main, strncmp failed);
						success = false;
					}
				}
			}
		}
	}

	// FORK IT

	// CLEAN UP
	if (silentBin)
	{
		if (false == free_rBinDat_ptr(&silentBin))
		{
			HARKLE_ERROR(redirect_bin_output, main, free_rBinDat_ptr failed);
		}
	}

	if (timestamp)
	{
		if (false == release_a_string(&timestamp))
		{
			HARKLE_ERROR(redirect_bin_output, main, release_a_string failed);
		}
	}

	// DONE
	if (success == false)
	{
		retVal = -1;
	}

	return retVal;
}


char* build_filename(char* timestamp, char* properName, bool output)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	bool success = true;  // Make this false if anything fails
	int fullLogLen = 0;  // Calculated string length of the return value

	// INPUT VALIDATION
	if (!timestamp || !properName)
	{
		HARKLE_ERROR(redirect_bin_output, build_filename, NULL pointer);
		success = false;
	}
	else if (!(*timestamp) || !(*properName))
	{
		HARKLE_ERROR(redirect_bin_output, build_filename, Empty string);
		success = false;
	}

	// BUILD FILENAME
	// 1. Determine necessary length
	fullLogLen = strlen(timestamp) + 1 + strlen(properName) + 1


	// CLEAN UP

	// DONE
	return retVal;
}