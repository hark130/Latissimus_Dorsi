#include "Harklerror.h"							// HARKLE_ERROR, HARKLE_ERRNO, HARKLE_WARNG
#include <stdbool.h>							// bool, true, false


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
	void* tmp_ptr = NULL;  // Return value from string.h function calls
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
		
		// Error Checking
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
		
		// Warnings
		if (!(srcLen % sizeof(void*)))
		{
			// I'm not sure if this is a big deal or not.
			// This may not ever happen.
			// If it ever does, I'm sure I should do something about it.
			// fprintf(stderr, "¿¿¿WARNING??? - htrace_read_data - The length of the 'blob' is not word-aligned");  // DEBUGGING
			HARKLE_WARNG(Harkletrace, htrace_read_data, The length of the 'blob' is not word-aligned);  // DEBUGGING
		}
		
		if (sizeof(void*) != sizeof(ptRetVal))
		{
			HARKLE_WARNG(Harkletrace, htrace_read_data, Size mismatch between a void* and a 'word');  // DEBUGGING
			// fprintf(stderr, "¿¿¿WARNING??? - htrace_read_data - Size mismatch between a void* and a 'word'");  // DEBUGGING	
		}
	}
	
	// ALLOCATE MEMORY
	if (true == success)
	{
		errno = 0;  // Reset errno
		retVal = get_me_memory(srcLen);
		
		if (!retVal)
		{
			if (errno)
			{
				*errNum = errno;
			}
			else
			{
				*errNum = ENOMEM;
			}
			
			HARKLE_ERROR(Harkletrace, htrace_read_data, get_me_memory failed);
			success = false;
		}
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
				// fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(*errNum));
				HARKLE_ERRNO(Harkletrace, ptrace, *errNum);
				success = false;
				break;
			}
			else
			{
				tmp_ptr = memcpy((*(retVal + i)), &ptRetVal, sizeof(ptRetVal));
				
				if (tmp_ptr != (*(retVal + i)))
				{
					*errNum = errno;
					HARKLE_ERROR(Harkletrace, htrace_read_data, memcpy failed);
					// fprintf(stderr, "memcpy() returned errno:\t%s\n", strerror(*errNum));
					HARKLE_ERRNO(Harkletrace, memcpy, *errNum);
					success = false;
					break;
				}
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
	bool unaligned = false;  // Set this to true if src_ptr's length violates the page alignment
	size_t i = 0;  // Iterating variable
	long ptRetVal = 0;  // Store the ptrace() return value here
	void* lastAddr = NULL;  // Staging area for last 'uneven' write
	
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
	else if (!(srcLen % sizeof(long)))
	{
		// I'm not sure if this is a big deal or not.
		// This may not ever happen.
		// If it ever does, I'm sure I should do something about it.
		// fprintf(stderr, "¿¿¿WARNING??? - htrace_write_data - The length of the 'blob' is not word-aligned.");  // DEBUGGING
		HARKLE_WARNG(Harkletrace, htrace_write_data, The length of the 'blob' is not word-aligned);  // DEBUGGING
		unaligned = true;
	}
	
	// LOOP PTRACE
	if (true == success)
	{
		for (i = 0; i < srcLen; i++)
		{
			// Handle that last awkward bit
			if (true == unaligned && i > (srcLen - (srcLen % sizeof(long))))
			{
				lastAddr = htrace_read_data(pid, dest_ptr + i, sizeof(long), &retVal);
				
				if (!lastAddr || retVal)
				{
					HARKLE_ERROR(Harkletrace, htrace_write_data, htrace_read_data failed);
					HARKLE_ERRNO(Harkletrace, htrace_read_data, retVal);				
					success = false;
					break;
				}
				else
				{
					if (lastAddr != memcpy(lastAddr, src_ptr, srcLen % sizeof(long)))
					{
						retVal = errno;
						HARKLE_ERROR(Harkletrace, htrace_write_data, memcpy failed);
						HARKLE_ERRNO(Harkletrace, memcpy, retVal);				
						success = false;
						break;						
					}
					else
					{
						ptRetVal = ptrace(PTRACE_POKEDATA, pid, dest_ptr + i, lastAddr);
					}
				}
			}
			else
			{
				ptRetVal = ptrace(PTRACE_POKEDATA, pid, dest_ptr + i, src_ptr + i);
			}
			
			if (ptRetVal == -1)
			{
				retVal = errno;
				HARKLE_ERROR(Harkletrace, htrace_write_data, ptrace failed);
				// fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(retVal));
				HARKLE_ERRNO(Harkletrace, ptrace, retVal);				
				success = false;
				break;
			}
		}
	}
	
	// CLEAN UP
	if (lastAddr)
	{
		if (false == release_a_string_len(&lastAddr, sizeof(long))
		{
			HARKLE_ERROR(Harkletrace, htrace_write_data, release_a_string_len failed);
		}
	}
	
	// DONE
	return retVal;
}
