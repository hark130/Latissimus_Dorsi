#include "Harklerror.h"								// HARKLE_ERROR
#include <stdbool.h>								// bool, true, false


/*
	Purpose - Read a 'blob' from a PID's memory address using ptrace(PTRACE_PEEKDATA)
	Input
		pid - The "tracee" PID (see: ptrace(2))
		src_ptr - Offset into the "tracee"s USER area
		srcLen - Length of the 'blob'
		errNum [Out] - Pointer to an integer in which to store errno on error
	Output
		On success, pointer to the 'blob' being read
		On failure, NULL is returned and errno is assigned to errNum
	Notes:
		The void* returned by this function is NOT nul-terminated
 */
void* htrace_read_data(pid_t pid, void* src_ptr, size_t srcLen, int* errNum)
{
	// LOCAL VARIABLES
	void* retVal = NULL;
	bool success = true;
	size_t i = 0;  // Iterating variable
	long ptRetVal = 0;  // Store the ptrace() return value here
	
	// INPUT VALIDATION
	if (!errNum)
	{
		HARKLE_ERROR(Harkletrace, htrace_read_data, NULL errNum pointer);
		success = false;
	}
	else
	{
		*errNum = 0;  // Clear errNum	
	
		if (pid < 1)
		{
			HARKLE_ERROR(Harkletrace, htrace_read_data, Invalid PID);
			success = false;
			*errNum = EINVAL;
		}
		else if (!src_ptr)
		{
			HARKLE_ERROR(Harkletrace, htrace_read_data, NULL pointer);
			success = false;
			*errNum = EINVAL;
		}
		else if (srcLen < 1)
		{
			HARKLE_ERROR(Harkletrace, htrace_read_data, Invalid length);
			success = false;
			*errNum = EINVAL;
		}
		else if (!(srcLen % sizeof(void*)))
		{
			// I'm not sure if this is a big deal or not.
			// This may not ever happen.
			// If it ever does, I'm sure I should do something about it.
			fprintf(stderr, "¿¿¿WARNING??? - htrace_read_data - The length of the 'blob' is not word-aligned.");  // DEBUGGING
		}
	}
	
	// ALLOCATE MEMORY
	if (true == success)
	{
		// Implement later
		success = false;  // Go no further
	}
	
	// LOOP PTRACE
	if (true == success)
	{
		for (i = 0; i < srcLen; i++)
		{
			ptRetVal = ptrace(PTRACE_PEEKDATA, pid, dest_ptr + i, NULL);
			
			if (ptRetVal == -1)
			{
				*errNum = errno;
				HARKLE_ERROR(Harkletrace, htrace_read_data, ptrace failed);
				fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(*errNum));
				success = false;
				break;
			}
			else
			{
				(*(retVal + i)) = ptRetVal;	
			}
		}		
	}
	
	// CLEAN UP
	if (false == success)
	{
		// retVal
		if (retVal)
		{
			if (false == release_a_string_len(&retVal, srcLen))
			{
				HARKLE_ERROR(Harkletrace, htrace_read_data, release_a_string_len failed);
			}
		}
	}
		
	// DONE
	return retVal;
}


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
		HARKLE_ERROR(Harkletrace, htrace_write_data, Invalid length);
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
