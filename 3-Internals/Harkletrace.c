#include <errno.h>								// errno
#include "Harklerror.h"							// HARKLE_ERROR, HARKLE_ERRNO, HARKLE_WARNG
#include "Memoroad.h"							// get_me_memory()
#include <stdbool.h>							// bool, true, false
#include <sys/ptrace.h>							// ptrace()
#include <sys/types.h>							// pid_t


void* htrace_read_data(pid_t pid, void* src_ptr, size_t srcLen, int* errNum)
{
	// LOCAL VARIABLES
	void* retVal = NULL;
	void* tmp_ptr = NULL;  // Return value from string.h function calls
	bool success = true;
	size_t i = 0;  // Iterating variable
	long ptRetVal = 0;  // Store the ptrace() return value here
	// size_t newSrcLen = 0;  // Word-align the source
	
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
		if (srcLen % sizeof(void*))
		{
			// I'm not sure if this is a big deal or not.
			// This may not ever happen.
			// If it ever does, I'm sure I should do something about it.
			// fprintf(stderr, "¿¿¿WARNING??? - htrace_read_data - The length of the 'blob' is not word-aligned");  // DEBUGGING
			HARKLE_WARNG(Harkletrace, htrace_read_data, The length of the 'blob' is not word-aligned);  // DEBUGGING
			// HARKLE_WARNG(Harkletrace, htrace_read_data, Padding the 'blob');  // DEBUGGING
			// fprintf(stdout, "Reading %zu bytes\n", srcLen);  // DEBUGGING
			// newSrcLen = srcLen + (srcLen % sizeof(void*));
			// fprintf(stdout, "Now at  %zu bytes\n", newSrcLen);  // DEBUGGING
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
		for (i = 0; i < (srcLen - sizeof(void*)); i++)  // Errors reading from the last 8 bytes?!
		// for (i = 0; i < srcLen; i++)
		{
			// fprintf(stdout, "Peeking into PID %d's %p which is %zu length.\n", pid, src_ptr + i, srcLen);  // DEBUGGING
			ptRetVal = ptrace(PTRACE_PEEKDATA, pid, src_ptr + i, NULL);
			
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
				tmp_ptr = memcpy(retVal + i, &ptRetVal, sizeof(ptRetVal));
				
				if (tmp_ptr != retVal + i)
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
			if (false == release_a_string_len((char**)&retVal, srcLen))
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
		for (i = 0; i < srcLen; i+= sizeof(long))
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
					if (lastAddr != memcpy(lastAddr, src_ptr + i, srcLen % sizeof(long)))
					{
						retVal = errno;
						HARKLE_ERROR(Harkletrace, htrace_write_data, memcpy failed);
						HARKLE_ERRNO(Harkletrace, memcpy, retVal);				
						success = false;
						break;						
					}
					else
					{
						fprintf(stdout, "htrace_write_data() is attempting to write this FINAL data:\t");  // DEBUGGING
						ptRetVal = ptrace(PTRACE_POKEDATA, pid, dest_ptr + i, lastAddr);
						for (int j = 0; j < sizeof(unsigned long); j++)
						{
							fprintf(stdout, "%02X", (*(((unsigned char*)lastAddr) + j)));  // DEBUGGING
						}
						fprintf(stdout, "\n");
					}
				}
			}
			else
			{
				/* DEBUGGING */
				// fprintf(stdout, "Attempting to write this data: ");  // DEBUGGING
				// for (int j = 0; j < sizeof(unsigned long); j++)
				// {
				// 	fprintf(stdout, "%02X", (*(((unsigned char*)dest_ptr) + i + j)));  // DEBUGGING
				// }
				// fprintf(stdout, " with this: ");
				// for (int j = 0; j < sizeof(unsigned long); j++)
				// {
				// 	fprintf(stdout, "%02X", (*(((unsigned char*)src_ptr) + i + j)));  // DEBUGGING
				// }

				// errno = 0;  // DEBUGGING... just a test

				// ptRetVal = ptrace(PTRACE_POKEDATA, pid, dest_ptr + i, src_ptr + i);
				ptRetVal = ptrace(PTRACE_POKETEXT, pid, dest_ptr + i, src_ptr + i);

				// HARKLE_ERRNO(Harkletrace, ptrace, errno);

				/* DEBUGGING */
				// fprintf(stdout, " and this happened: ");
				// for (int j = 0; j < sizeof(unsigned long); j++)
				// {
				// 	fprintf(stdout, "%02X", (*(((unsigned char*)dest_ptr) + i + j)));  // DEBUGGING
				// }
				// fprintf(stdout, "\n");
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
		if (false == release_a_string_len((char**)&lastAddr, sizeof(long)))
		{
			HARKLE_ERROR(Harkletrace, htrace_write_data, release_a_string_len failed);
		}
	}
	
	// DONE
	return retVal;
}


/*
	Purpose - Match a snippet of memory (needle) in a PID's larger 'blob' of memory
	Input
		pid - The PID of the target process
		haystack_ptr - A pointer to a memory area of length haystackLen
		needle_ptr - A pointer to a memory area of length needleLen
		haystackLen - The size of the memory area haystack_ptr points to
		needleLen - The size of the memory area needle_ptr points to
	Output
		On success...
			Offset to the first occurrence of needle_ptr in haystack_ptr
			-or-
			-1 if needle_ptr is not found in haystack_ptr
		On failure, -2
	Notes:
		If this function sounds like strstr() and memcmp() had a child and then married
			ptrace(PTRACE_POKEDATA), then you understand what I'm trying to do here.
 */
size_t pid_mem_hunt(pid_t pid, void* haystack_ptr, void* needle_ptr, size_t haystackLen, size_t needleLen)
{
	// LOCAL VARIABLES
	size_t retVal = -1;
	bool success = true;  // Make this false if anything fails
	// void* tempRetVal = NULL;  // Store string.h function calls here
	size_t i = 0;  // Iterating variable
	void* pidHaystack_ptr = NULL;  // Read the PID's memory here
	int errNum = 0;  // Store errnos here
	
	// INPUT VALIDATION
	if (pid < 1)
	{
		HARKLE_ERROR(Harkletrace, pid_mem_hunt, Invalid PID);
		success = false;
	}
	else if (!haystack_ptr || !needle_ptr)
	{
		HARKLE_ERROR(Harkletrace, pid_mem_hunt, NULL pointer);
		success = false;
	}
	else if (haystackLen < 1 || needleLen < 1 || needleLen > haystackLen)
	{
		HARKLE_ERROR(Harkletrace, pid_mem_hunt, Invalid length);
		success = false;
	}

	// READ HAYSTACK LOCALLY
	if (true == success)
	{
		pidHaystack_ptr = htrace_read_data(pid, haystack_ptr, haystackLen, &errNum);

		if (!pidHaystack_ptr && errNum)
		{
			HARKLE_ERROR(Harkletrace, pid_mem_hunt, htrace_read_data failed);
			HARKLE_ERRNO(Harkletrace, htrace_read_data, errNum);
			success = false;
			retVal = -2;
		}
		else if (pidHaystack_ptr && !errNum)
		{
			if (haystack_ptr > pidHaystack_ptr)
			{
				HARKLE_ERROR(Harkletrace, pid_mem_hunt, htrace_read_data failed out of bounds);
				HARKLE_ERRNO(Harkletrace, htrace_read_data, errNum);
				success = false;
			retVal = -2;
			}
			else
			{
				retVal = pidHaystack_ptr - haystack_ptr;
			}
		}
		else  // How did we get here?!
		{
			HARKLE_ERROR(Harkletrace, pid_mem_hunt, Haystack logic failed);
			HARKLE_ERRNO(Harkletrace, htrace_read_data, errNum);
			success = false;
			retVal = -2;
		}
	}

	// FIND IT
	if (true == success)
	{
		for (i = 0; i <= (haystackLen - needleLen); i++)
		{
			if (0 == memcmp(pidHaystack_ptr + i, needle_ptr, needleLen))
			{
				retVal = i;
				break;
			}
		}
	}

	// CLEAN UP
	if (pidHaystack_ptr)
	{
		if (false == release_a_string_len((char**)&pidHaystack_ptr, haystackLen))
		{
			HARKLE_ERROR(Harkletrace, pid_mem_hunt, release_a_string_len failed);
		}
	}
	
	// DONE
	return retVal;
}
