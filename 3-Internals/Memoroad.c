#define _GNU_SOURCE							// process_vm_readv() and process_vm_writev() are only available when GNU extensions are enabled
// #include <asm/page.h>						// PAGE_SIZE
// #include <asm-generic/page.h>				// PAGE_SIZE
#include <errno.h>							// errno
#include "Harklerror.h"						// HARKLE_ERROR
#include "Memoroad.h"
#include <stdbool.h>						// bool, true, false
#include <stdio.h>							// fprintf
#include <stdlib.h>							// calloc
#include <string.h>							// memset, memcpy
#include <sys/mman.h>						// mprotect()
#include <sys/uio.h>						// process_vm_readv(), process_vm_writev()
#include <unistd.h>							// sysconf()

#ifndef MEMOROAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define MEMOROAD_MAX_TRIES 3
#endif  // MEMOROAD_MAX_TRIES

#ifndef MEMSET_DEFAULT
#define MEMSET_DEFAULT 0x0
#endif  // MEMSET_DEFAULT

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Allocate a buffer of size length
	Input
		length - The length of what you want to store
	Ouput - Heap-allocated, memset, buffer of size length
	Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
		There is no extra space allocated in this buffer
 */
void* get_me_memory(size_t length)
{
    // LOCAL VARIABLES
    int numTries = 0;  // Max number to calloc attempts
    char* retVal = NULL;  // Allocated char array
    char* temp_ptr = NULL;  // Holds string.h function return values
    bool success = true;  // If anything fails, this is becomes false

    // INPUT VALIDATION
    if (length < 1)
    {
    	HARKLE_ERROR(Memoroad, get_me_memory, Invalid buffer length);
    	success = false;
    }

    // ALLOCATION
    if (success == true)
    {
        while (retVal == NULL && numTries < MEMOROAD_MAX_TRIES)
        {
            retVal = (char*)calloc(length + 1, sizeof(char));
            numTries++;
        }

        if (!retVal)
        {
	    	HARKLE_ERROR(Memoroad, get_me_memory, calloc failed);
	    	success = false;
        }
    }

    // DONE
    return retVal;
}


char* get_me_a_buffer(size_t length)
{
    // LOCAL VARIABLES
    char* retVal = NULL;  // Allocated char array
    bool success = true;  // If anything fails, this is becomes false

    // INPUT VALIDATION
    if (length < 1)
    {
    	HARKLE_ERROR(Memoroad, get_me_a_buffer, Invalid buffer length);
    	success = false;
    }

    // ALLOCATION
    if (success == true)
    {
		retVal = (char*)get_me_memory(length + 1);
		
		if (!retVal)
		{
	    	HARKLE_ERROR(Memoroad, get_me_a_buffer, get_me_memory failed);
	    	success = false;
		}
    }

    // DONE
    return retVal;
}


char** get_me_a_buffer_array(size_t arraySize, bool nullTerm)
{
	// LOCAL VARIABLES
	char** retVal = NULL;
	bool success = true;
	int numTries = 0;  // Count the number of allocation attempts
	size_t actualArrSize = arraySize;
	
	// INPUT VALIDATION
	if (arraySize < 1)
	{
		HARKLE_ERROR(Memoroad, get_me_a_buffer_array, Invalid array size);
		success = false;
	}
	
	// ALLOCATE
	if (success == true)
	{
		// NULL terminate?
		if (nullTerm == true)
		{
			actualArrSize++;
		}
		
		// Allocate
		while (!retVal && numTries < MEMOROAD_MAX_TRIES)
		{
			retVal = (char**)calloc(actualArrSize, sizeof(char*));
			numTries++;
		}
		
		if (!retVal)
		{
			HARKLE_ERROR(Memoroad, get_me_a_buffer_array, calloc failed);
			success = false;
		}
	}
	
	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_char_arr(&retVal))
			{
				HARKLE_ERROR(Memoroad, get_me_a_buffer_array, free_char_arr failed);
			}
		}
	}
	
	// DONE
	return retVal;
}


char* copy_a_string(const char* char_ptr)
{
    // LOCAL VARIABLES
    char* retVal = NULL;  // Allocated char array
    char* temp_ptr = NULL;  // Holds return value from memcpy
    size_t charLen = 0;  // Length of char_ptr
    
    // INPUT VALIDATION
    if (char_ptr)
    {
        if (*char_ptr)
        {    
            // 1. Size the char_ptr
            charLen = strlen(char_ptr);
            
            if (charLen > 0)
            {            
                // 2. Allocate a properly-sized array
            	retVal = get_me_a_buffer(charLen);
                
                if (retVal)
                {                
                    // 3. Copy char_ptr into that array
                    temp_ptr = (char*)memcpy((void*)retVal, (void*)char_ptr, charLen * sizeof(char));
                    
                    if (temp_ptr != retVal)
                    {
                        // fprintf(stderr, "<<<ERROR>>> - Fileroad - copy_a_string() - memcpy failed!\n");
        				HARKLE_ERROR(Fileroad, copy_a_string, memcpy failed);
                        // Free retVal
                        free(retVal);
                        // NULL retVal
                        retVal = NULL;
                    }
                }
                else
                {
        			HARKLE_ERROR(Fileroad, copy_a_string, get_me_a_buffer failed);
                }                
            }
        }
    }
    
    // DONE
    return retVal;
}


struct iovec* allocate_iovec_struct(void)
{
    // LOCAL VARIABLES
    int numTries = 0;  // Max number to calloc attempts
    struct iovec* retVal = NULL;
    void* temp_ptr = NULL;  // Holds string.h function return values
    bool success = true;  // If anything fails, this is becomes false

    // ALLOCATION
    while (retVal == NULL && numTries < MEMOROAD_MAX_TRIES)
    {
        retVal = (struct iovec*)calloc(1, sizeof(struct iovec));
        numTries++;
    }

    if (!retVal)
    {
    	HARKLE_ERROR(Memoroad, allocate_iovec_struct, calloc failed);
    	success = false;
    }

    // DONE
    return retVal;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


bool release_a_string(char** charPtr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char* char_ptr = NULL;
	size_t stringLen = 0;  // Attempt to get the string length

	if (charPtr_ptr)
	{
		if (*charPtr_ptr)
		{
			retVal = release_a_string_len(charPtr_ptr, strlen(*charPtr_ptr));
		}
		else
		{
			HARKLE_ERROR(Fileroad, release_a_string, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Fileroad, release_a_string, NULL pointer);
		retVal = false;
	}

	// DONE
	return retVal;
}


bool release_a_string_len(char** charPtr_ptr, size_t buffSize)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Default... prove it wrong
	char* char_ptr = NULL;  // Easier to deal with it this way
	char* temp_ptr = NULL;  // Return value from string.h function calls

	// INPUT VALIDATION
	if (!charPtr_ptr)
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, NULL pointer);
		retVal = false;
	}
	else if (!(*charPtr_ptr))
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, NULL pointer);
		retVal = false;
	}
	else if (buffSize < 0)
	{
		HARKLE_ERROR(Fileroad, release_a_string_len, Invalid buffer size);
		retVal = false;
	}

	if (retVal == true)
	{
		// RELEASE THE STRING
		char_ptr = *charPtr_ptr;

		if (buffSize > 0)
		{
			// 1. memset
			// fprintf(stdout, "release_a_string_len() - About to memset() char_ptr %s (%p)\n", char_ptr, char_ptr);
			temp_ptr = (void*)memset((void*)char_ptr, MEMSET_DEFAULT, buffSize);
			// fprintf(stdout, "release_a_string_len() - Just memset() char_ptr %s (%p)\n", char_ptr, char_ptr);

			if (temp_ptr != char_ptr)
			{
				HARKLE_ERROR(Fileroad, release_a_string_len, memset failed);
			}
		}

		// 2. free
		// fprintf(stdout, "release_a_string_len() - About to free() char_ptr (%p)\n", char_ptr);
		free(char_ptr);

		// 3. NULL
		char_ptr = NULL;
		*charPtr_ptr = NULL;
	}

	// DONE
	return retVal;
}


bool free_char_arr(char*** charArr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char* currChar_ptr = NULL;  // C-string to be cleared
	char* temp_ptr = NULL;  // Return value from memset
	char** currChar_arr = NULL;  // Array of C-strings to be cleared
	size_t currLen = 0;  // Length of the current string

	// INPUT VALIDATION
	if (charArr_ptr)
	{
		if (*charArr_ptr)
		{
			currChar_arr = *charArr_ptr;

			while (*currChar_arr)
			{
				currChar_ptr = *currChar_arr;

				if (currChar_ptr)
				{
					// fprintf(stdout, "About to memset (%p):\t%s\n", currChar_ptr, currChar_ptr);  // DEBUGGING
					// memset char*
					if (*currChar_ptr)
					{
						currLen = strlen(currChar_ptr);

						if (currLen > 0)
						{
							temp_ptr = memset(currChar_ptr, MEMSET_DEFAULT, currLen);

							if (temp_ptr != currChar_ptr)
							{
								HARKLE_ERROR(Memoroad, free_char_arr, memset failed);
								retVal = false;
							}
							else
							{
								temp_ptr = NULL;
							}
						}
					}

					// free char*
					// free(*currChar_arr);
					free(currChar_ptr);

					// NULL char*
					currChar_ptr = NULL;
					*currChar_arr = NULL;
				}
				// Next char*
				currChar_arr++;
			}

			// free char**
			free(*charArr_ptr);

			// NULL char**
			charArr_ptr = NULL;
		}
		else
		{
			HARKLE_ERROR(Memoroad, free_char_arr, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Memoroad, free_char_arr, NULL pointer);
		retVal = false;
	}

	// DONE
	return retVal;
}


bool free_iovec_struct(struct iovec** oldStruct_ptr, bool freeAll)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Default... prove it wrong
	struct iovec* iovec_ptr = NULL;  // Easier to deal with it this way
	void* temp_ptr = NULL;  // Return value from string.h function calls

	// INPUT VALIDATION
	if (!oldStruct_ptr)
	{
		HARKLE_ERROR(Fileroad, free_iovec_struct, NULL pointer);
		retVal = false;
	}
	else if (!(*oldStruct_ptr))
	{
		HARKLE_ERROR(Fileroad, free_iovec_struct, NULL pointer);
		retVal = false;
	}
	else
	{
		iovec_ptr = *oldStruct_ptr;
	}

	if (true == retVal)
	{
		// 1. Clear void *iov_base;    /* Starting address */
		if (true == freeAll && iovec_ptr->iov_base)
		{
			// 1.1. memset the memory
			if (iovec_ptr->iov_len > 0)
			{
				temp_ptr = memset(iovec_ptr->iov_base, MEMSET_DEFAULT, iovec_ptr->iov_len);

				if (temp_ptr != iovec_ptr->iov_base)
				{
					HARKLE_ERROR(Memoroad, free_iovec_struct, memset failed);
					retVal = false;
				}
				else
				{
					temp_ptr = NULL;
				}
			}
			// 1.2. free() the memory
			free(iovec_ptr->iov_base);
		}
		iovec_ptr->iov_base = NULL;

		// 2. Clear size_t iov_len;     /* Number of bytes to transfer */
		iovec_ptr->iov_len = 0;

		// 3. Free the struct pointer
		free(iovec_ptr);

		// 4. NULL the variables
		iovec_ptr = NULL;
		*oldStruct_ptr = NULL;
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////// MEM TRANSFER FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


struct iovec* copy_remote_to_local(pid_t pid, void* remoteMem, size_t numBytes)
{
	// LOCAL VARIABLES
	struct iovec* retVal = NULL;
	bool success = true;  // Make this false if anything fails
	int numTries = 0;  // Keep count of allocation attempts
	ssize_t pvrRetVal = 0;  // Return value from process_vm_readv() call
	struct iovec remMem[1];  // iovec struct array to hold the remote copy info
	int errNum = 0;  // Capture errno here

	// INPUT VALIDATION
	if (pid < 1)
	{
		HARKLE_ERROR(Memoroad, copy_remote_to_local, Invalid PID);
		success = false;
	}
	else if (!remoteMem)
	{
		HARKLE_ERROR(Memoroad, copy_remote_to_local, NULL pointer);
		success = false;
	}
	else if (numBytes < 1)
	{
		HARKLE_ERROR(Memoroad, copy_remote_to_local, Invalid number of bytes);
		success = false;
	}

	// ALLOCATE A STRUCT
	if (true == success)
	{
		retVal = allocate_iovec_struct();

		if (!retVal)
		{
			HARKLE_ERROR(Memoroad, copy_remote_to_local, allocate_iovec_struct failed);
			success = false;
		}
	}

	// ALLOCATE MEMORY
	if (true == success)
	{
		while (NULL == retVal->iov_base && numTries < MEMOROAD_MAX_TRIES)
		{
			retVal->iov_base = (void*)calloc(numBytes, 1);

			if (retVal->iov_base)
			{
				retVal->iov_len = numBytes;
				numTries++;
			}
		}

		if (NULL == retVal->iov_base)
		{
			HARKLE_ERROR(Memoroad, copy_remote_to_local, calloc failed);
			success = false;
		}
	}

	// COPY THE MEMORY
	if (true == success)
	{
		// Prepare the remote struct pointer
		remMem[0].iov_base = remoteMem;
		remMem[0].iov_len = numBytes;

		// ssize_t process_vm_readv(pid_t pid,
		//                          const struct iovec *local_iov,
		//                          unsigned long liovcnt,
		//                          const struct iovec *remote_iov,
		//                          unsigned long riovcnt,
		//                          unsigned long flags);
		pvrRetVal = process_vm_readv(pid, retVal, 1, remMem, 1, 0);

		if (-1 == pvrRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(Memoroad, copy_remote_to_local, process_vm_readv failed);
			fprintf(stderr, "process_vm_readv() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else if (pvrRetVal != numBytes)
		{
			HARKLE_ERROR(Memoroad, copy_remote_to_local, process_vm_readv only made a partial read);
			success = false;
		}
	}

	// CLEAN UP
	if (false == success)
	{
		if (retVal)
		{
			if (false == free_iovec_struct(&retVal, true))
			{
				HARKLE_ERROR(Memoroad, copy_remote_to_local, free_iovec_struct failed);
			}
		}
	}

	// DONE
	return retVal;
}


int copy_local_to_remote(pid_t pid, void* remoteMem, void* localMem, size_t numBytes)
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;  // Make this false if anything fails
	int numTries = 0;  // Keep count of allocation attempts
	ssize_t pvwRetVal = 0;  // Return value from process_vm_writev() call
	struct iovec locMem[1];  // iovec struct array to hold the local memory info
	struct iovec remMem[1];  // iovec struct array to hold the remote memory info

	// INPUT VALIDATION
	if (pid < 1)
	{
		HARKLE_ERROR(Memoroad, copy_local_to_remote, Invalid PID);
		success = false;
	}
	else if (!remoteMem || !localMem)
	{
		HARKLE_ERROR(Memoroad, copy_local_to_remote, NULL pointer);
		success = false;
	}
	else if (numBytes < 1)
	{
		HARKLE_ERROR(Memoroad, copy_local_to_remote, Invalid number of bytes);
		success = false;
	}
	
	// COPY THE MEMORY
	if (true == success)
	{
		// Prepare the local struct pointer
		locMem[0].iov_base = localMem;
		locMem[0].iov_len = numBytes;
		
		// Prepare the remote struct pointer
		remMem[0].iov_base = remoteMem;
		remMem[0].iov_len = numBytes;

		// ssize_t process_vm_writev(pid_t pid,
		//                           const struct iovec *local_iov,
		//                           unsigned long liovcnt,
		//                           const struct iovec *remote_iov,
		//                           unsigned long riovcnt,
		//                           unsigned long flags);
		pvwRetVal = process_vm_writev(pid, locMem, 1, remMem, 1, 0);
		
		if (-1 == pvwRetVal)
		{
			retVal = errno;
			HARKLE_ERROR(Memoroad, copy_local_to_remote, process_vm_writev failed);
			fprintf(stderr, "process_vm_writev() returned errno:\t%s\n", strerror(retVal));
			success = false;
		}
		else if (pvwRetVal != numBytes)
		{
			HARKLE_ERROR(Memoroad, copy_local_to_remote, process_vm_writev only made a partial write);
			success = false;
		}
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////// MEM TRANSFER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
///////////////////////// MAPPED MEM FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


int change_mmap_prot(void* mem_ptr, size_t memLen, int newProt)
{
	// LOCAL VARIABLES
	int retVal = 0;
	long pageSize = get_page_size();
	
	// INPUT VALIDATION
	if (!mem_ptr)
	{
		HARKLE_ERROR(Memoroad, change_mmap_prot, NULL pointer);
		retVal = EINVAL;
	}
	else if (memLen < 1)
	{
		HARKLE_ERROR(Memoroad, change_mmap_prot, Invalid page length);
		retVal = EINVAL;
	}
	else if (pageSize > 0 && (memLen % pageSize))
	{
		HARKLE_ERROR(Memoroad, change_mmap_prot, Memory length is not page aligned);
		retVal = EINVAL;
	}
	else if (newProt && 0 == (newProt & \
			 (MROAD_PROT_NONE | MROAD_PROT_READ | MROAD_PROT_WRITE | \
			  MROAD_PROT_EXEC | MROAD_PROT_SEM | MROAD_PROT_SAO | \
			  MROAD_PROT_GROWSUP | MROAD_PROT_GROWSDOWN)))
	{
		HARKLE_ERROR(Memoroad, change_mmap_prot, Unrecognized protection setting);
		retVal = EINVAL;
	}
	
	// SYSTEM CALL
	if (0 == retVal)
	{
		retVal = mprotect(mem_ptr, memLen, newProt);
		
		if (-1 == retVal)
		{
			retVal = errno;
			HARKLE_ERROR(Memoroad, change_mmap_prot, mprotect failed);
			fprintf(stderr, "mprotect() returned errno:\t%s\n", strerror(retVal));			
		}		
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////// MAPPED MEM FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


long get_page_size(void)
{
	// LOCAL VARIABLES
	long retVal = -1;
	int errNum = 0;  // Store errno here in an error condition
	errno = 0;  // Zeroize errno prior to the call for the best determine results
	
	// SYSTEM CALL
	retVal = sysconf(_SC_PAGE_SIZE);
	
	if (-1 == retVal)
	{
		errNum = errno;
		if (0 == errNum)
		{
			HARKLE_ERROR(Memoroad, get_page_size, sysconf indicated the limit was indeterminate);
			retVal = -2;
		}
		else
		{
			HARKLE_ERROR(Memoroad, get_page_size, sysconf failed);
			// fprintf(stderr, "sysconf() returned errno:\t%s\n", strerror(errNum));
			HARKLE_ERRNO(Memoroad, sysconf, errNum);
			retVal = -1;
		}

#ifdef PAGE_SIZE
		retVal = PAGE_SIZE;
#endif  // PAGE_SIZE
	}
		
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////
