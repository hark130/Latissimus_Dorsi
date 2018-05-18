#include "Harklerror.h"								// HARKLE_ERROR
#include <stdbool.h>								// bool, true, false


int htrace_write_data(pid_t pid, void* dest_ptr, void* src_ptr, size_t srcLen)
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;
	size_t i = 0;  // Iterating variable
	long ptRetVal = 0;  // Store the ptrace() return value here
	
	// INPUT VALIDATION
	if (pid < 1)
	{
		HARKLE_ERROR(Harkletrace, htrace_write_data, Invalid PID);
		success = false;
		retVal = EINVAL;
	}
	else if (!dest_ptr || !src_ptr)
	{
		HARKLE_ERROR(Harkletrace, htrace_write_data, NULL pointer);
		success = false;
		retVal = EINVAL;
	}
	else if (srcLen < 1)
	{
		HARKLE_ERROR(Harkletrace, htrace_write_data, NULL pointer);
		success = false;
		retVal = EINVAL;
	}
	else if (!(srcLen % sizeof(void*)))
	{
		// I'm not sure if this is a big deal or not.
		// This may not ever happen.
		// If it ever does, I'm sure I should do something about it.
		fprintf(stderr, "¿¿¿WARNING??? - htrace_write_data - The length of the 'blob' is not word-aligned.");  // DEBUGGING
	}
	
	// LOOP PTRACE
	if (true == success)
	{
		for (i = 0; i < srcLen; i++)
		{
			ptRetVal = ptrace(PTRACE_POKEDATA, pid, dest_ptr + i, src_ptr + i);
			
			if (ptRetVal == -1)
			{
				retVal = errno;
				HARKLE_ERROR(Harkletrace, htrace_write_data, ptrace failed);
				fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(retVal));
				success = false;
				break;
			}
		}
	}	
	
	// DONE
	return retVal;
}
