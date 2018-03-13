#include <errno.h>			// errno
#include "Harklerror.h"		// HARKLE_ERROR
#include <inttypes.h>		// uintmax_t
#include "Memoroad.h"		// get_me_a_buffer
#include <stdbool.h>		// bool, true, false
#include <stdio.h>			// fprintf
#include <string.h>			// strerror
#include <time.h>			// time, localtime


struct tm* get_localtime(void)
{
	// LOCAL VARIABLES
	struct tm* retVal = NULL;
	bool success = true;
	time_t lTime = 0;  // Holds return value from time()

	// // INPUT VALIDATION
	// if (!timeDateStruct)
	// {
	// 	HARKLE_ERROR(Timeroad, get_localtime, NULL pointer);
	// 	success = false;
	// }

	// TIME
	// 1. Get the number of seconds since Epoch
	if (success == true)
	{
		lTime = time(NULL);

		if (lTime == -1)
		{
			HARKLE_ERROR(Timeroad, get_localtime, time failed);
			success = false;
		}
	}

	// 2. Convert Epoch to time date strucct
	if (success = true)
	{
		retVal = localtime(&lTime);

		if (!retVal)
		{
			HARKLE_ERROR(Timeroad, get_localtime, localtime failed);
			success = false;
		}
		else
		{
			// fprintf(stdout, "%s\n%ju secs since the Epoch\n", asctime(retVal), (uintmax_t)lTime);  // DEBUGGING
		}		
	}

	// DONE
	return retVal;
}


char* build_timestamp(void)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	bool success = true;
	int dtLen = 14 + 1;  // 14 for numbers, 1 for dash
	int printRetVal = 0;  // sprintf return value
	struct tm* dtStruct = get_localtime();

	// INPUT VALIDATION
	if (!dtStruct)
	{
		success = false;
	}

	// PROCESS STRING
	if (success == true)
	{
		// 1. Allocate a buffer
		retVal = get_me_a_buffer(dtLen);

		if (!retVal)
		{
			HARKLE_ERROR(Timeroad, build_timestamp, get_me_a_buffer failed);
			success = false;
		}
		else
		{
			printRetVal = sprintf(retVal,"%04d%02d%02d-%02d%02d%02d", \
				    			  dtStruct->tm_year+1900, dtStruct->tm_mon, \
				    			  dtStruct->tm_mday, dtStruct->tm_hour, \
				    			  dtStruct->tm_min, dtStruct->tm_sec);

			if (printRetVal != dtLen)
			{
				HARKLE_ERROR(Timeroad, build_timestamp, sprintf count mismatch);
				success = false;
			}
			else if (strlen(retVal) != dtLen)
			{
				HARKLE_ERROR(Timeroad, build_timestamp, Unexpected timestamp length);
				success = false;
			}
			else
			{
				// fprintf(stdout, "Timestamp is %s\n", retVal);  // DEBUGGING
			}
		}
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string_len(&retVal, dtLen))
			{
				HARKLE_ERROR(Timeroad, build_timestamp, release_a_string_len failed);
			}	
		}
	}

	// DONE
	return retVal;
}
