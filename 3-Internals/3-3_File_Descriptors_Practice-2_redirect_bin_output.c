#include "Fileroad.h"
#include "Fileroad_Descriptors.h"
#include "Harklerror.h"
#include <stdio.h>
#include "Timeroad.h"


int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	int i = 0;  // Iterating variable
	rBinDat_ptr silentBin = NULL;  // Return value from build_rBinDat_ptr()
	char* timestamp = NULL;  // Return value from build_timestamp() goes here
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
				// 1234567890123456 +  strlen() +12345678901
				fullLogLen = 15 + 1 + strlen(argv[1]) + 11;
				// 3.2. Allocate arrays for both filenames
				silentBin->outputFile = 
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

	// DONE
	if (success == false)
	{
		retVal = -1;
	}

	return retVal;
}
