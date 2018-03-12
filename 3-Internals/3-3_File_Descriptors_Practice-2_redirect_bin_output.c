#include "Fileroad_Descriptors.h"
#include "Harklerror.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	int i = 0;  // Iterating variable
	rBinDat_ptr silentBin = NULL;  // Return value from build_rBinDat_ptr()

	// INPUT VALIDATION
	if (argc < 2)
	{		
		fprintf(stderr, "\nToo few arguments!\nusage: redirect_bin_output.exe /path/to/bin -arg1 -arg2\n\n");
		success = false;
	}
	else
	{
		for (i = 1; i < argc; i++)
		{
			if (*(argv[i]) == '\0')
			{
				HARKLE_ERROR(redirect_bin_output, main, Empty argument);
				success = false;
			}
		}
		
		// fprintf(stdout, "silentBin->binName == %s\n", argv[1]);  // DEBUGGING
	}

	// FORK IT
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
			// fprintf(stdout, "silentBin->binName == %s\n", silentBin->binName);  // DEBUGGING
		}
	}

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
