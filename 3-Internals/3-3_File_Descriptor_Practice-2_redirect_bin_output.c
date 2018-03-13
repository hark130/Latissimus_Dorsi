#include "Fileroad.h"
#include "Fileroad_Descriptors.h"
#include "Harklerror.h"
#include "Memoroad.h"			// get_me_a_buffer
#include <stdio.h>
#include <string.h>				// strlen
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
				// fprintf(stdout, "Timestamp is %s\n", timestamp);  // DEBUGGING

				// 3. Build the output/errors filenames
				// 3.1. outputFile
				if (success == true && silentBin->outputFile)
				{
					HARKLE_ERROR(redirect_bin_output, main, silentBin->outputFile already exists?);
					success = false;
				}
				else
				{
					silentBin->outputFile = build_filename(timestamp, silentBin->binName, true);
				}
				// 3.2. errorsFile
				if (success == true && silentBin->errorsFile)
				{
					HARKLE_ERROR(redirect_bin_output, main, silentBin->errorsFile already exists?);
					success = false;
				}
				else
				{
					silentBin->errorsFile = build_filename(timestamp, silentBin->binName, false);
				}

				// 3.3. Verify
				if (success == true && (!(silentBin->outputFile) || !(silentBin->errorsFile)))
				{
					HARKLE_ERROR(redirect_bin_output, main, build_filename failed);
					success = false;
				}
				
				fprintf(stdout, "silentBin->binName == %s\n", silentBin->binName);  // DEBUGGING
				fprintf(stdout, "silentBin->binPath == %s\n", silentBin->binPath);  // DEBUGGING
				fprintf(stdout, "silentBin->outputFile == %s\n", silentBin->outputFile);  // DEBUGGING
				fprintf(stdout, "silentBin->errorsFile == %s\n", silentBin->errorsFile);  // DEBUGGING
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
	char* temp_ptr = NULL;  // Return values from string.h function calls
	// char* curr_ptr = NULL;  // Iterate the way through retVal
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
	if (success == true)
	{
		// 1. Determine necessary length
		//             YYYYMMDD-HHMMSS + - +        properName  + - + output.txt
		fullLogLen = strlen(timestamp) + 1 + strlen(properName) + 1 + 10;

		// 2. Allocate a buffer
		retVal = get_me_a_buffer(fullLogLen);

		if (!retVal)
		{
			HARKLE_ERROR(redirect_bin_output, build_filename, get_me_a_buffer failed);
			success = false;
		}
		else
		{
			// 3. Build the output string
			// 3.1. Timestamp
			temp_ptr = strcpy(retVal, timestamp);

			if (temp_ptr != retVal)
			{
				HARKLE_ERROR(redirect_bin_output, build_filename, strncpy failed);
				success = false;
			}
			else
			{
				// 3.2. Dash
				temp_ptr = strcat(retVal, "-");

				if (temp_ptr != retVal)
				{
					HARKLE_ERROR(redirect_bin_output, build_filename, strcat failed);
					success = false;
				}
				else
				{
					// 3.3. properName
					temp_ptr = strcat(retVal, properName);

					if (temp_ptr != retVal)
					{
						HARKLE_ERROR(redirect_bin_output, build_filename, strcat failed);
						success = false;
					}
					else
					{
						// 3.4. Filename suffix
						if (output == true)
						{
							temp_ptr = strcat(retVal, "-output.txt");
						}
						else
						{
							temp_ptr = strcat(retVal, "-errors.txt");
						}

						if (temp_ptr != retVal)
						{
							HARKLE_ERROR(redirect_bin_output, build_filename, strcat failed);
							success = false;
						}
					}
				}

			}
		}
	}

	// CLEAN FILENAME
	if (success == true)
	{
		temp_ptr = clean_filename(retVal, true);

		if (temp_ptr != retVal)
		{
			HARKLE_ERROR(redirect_bin_output, build_filename, clean_filename failed);
			success = false;
		}
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string(&retVal))
			{
				HARKLE_ERROR(redirect_bin_output, build_filename, release_a_string failed);
			}
		}
	}

	// DONE
	return retVal;
}
