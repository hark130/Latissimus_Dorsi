#include "Harklepipe.h"		// build_a_pipe()
#include "Memoroad.h"		// copy_a_string(), get_me_a_buffer()
#include <string.h>			// memcpy(), strerror()

#ifndef HARKLETHREAD_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HARKLETHREAD_MAX_TRIES 3
#endif // HARKLETHREAD_MAX_TRIES

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


hThrDetails_ptr create_a_hThrDetails_ptr(char* threadName, \
	                                     int threadNum, \
	                                     void* (*start_routine) (void*), \
	                                     void* arg, \
	                                     size_t argSize)
{
	// LOCAL VARIABLES
	hThrDetails_ptr retVal = NULL;
	bool success = true;  // If anything fails, make this false
	void* tmp_ptr = NULL;  // Return value from string.h function calls
	int tmpInt = 0;  // Return value from build_a_pipe()
	
	// INPUT VALIDATION
	
	// ALLOCATE MEMORY
	if (true == success)
	{
		retVal = allocate_a_hThrDetails_ptr();
		
		if (!retVal)
		{
			HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, allocate_a_hThrDetails_ptr failed);
			success = false;
		}
	}
	
	// POPULATE THE STRUCT
	// 1. Basic copies 
	if (true == success)
	{
		// 1.1. threadName [optional]
		if (threadName && *threadName)
		{
			retVal->tName = copy_a_string(threadName);
		}
		
		// 1.2. threadNum [optional]
		if (threadNum)
		{
			retVal->tNum = threadNum;
		}
		
		// 1.3. start_routine
		retVal->strtFunc = start_routine;
		
		// 1.4. arg
		retVal->tArgvString = (void*)get_me_a_buffer(argSize - 1);
		
		if (!(retVal->tArgvString))
		{
			HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, get_me_a_buffer failed);
			success = false;
		}
		else
		{
			tmp_ptr = memcpy(retVal->tArgvString, arg, argSize);
			
			if (tmp_ptr != retVal->tArgvString)
			{
				HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, memcpy failed);
				success = false;
			}
			else
			{
				// 1.5. argSize
				retVal->tArgSize = argSize;
			}
			
			tmp_ptr = NULL;  // Reset temp variable
		}
	}
	
	// 2. Initialize the mutex
	if (true == success)
	{
		// 1. pthread_mutex_t pipeMutex; // Thread's pipe mutex
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
		// 2. pthread_mutexattr_t pipeMutexAttr; // Attributes for thread's pipe mutex
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
	}
	
	// 3. Build-A-Pipe Workshop
	if (true == success)
	{
		tmpInt = make_a_pipe(retVal->pipeFDs, 0);
		
		if (tmpInt)
		{
			HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, make_a_pipe failed);
			fprintf(stderr, "make_a_pipe() returned errno:\t%s\n", strerror(tmpInt));
			success = false;
		}
	}
	
	// CLEAN UP
	if (false == success && retVal)
	{
		if (false == free_a_hThrDetails_ptr(&retVal))
		{
			HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, free_a_hThrDetails_ptr failed);
		}
	}	

	// DONE
	return retVal;
}


hThrDetails_ptr allocate_a_hThrDetails_ptr(void)
{
	// LOCAL VARIABLES
	hThrDetails_ptr retVal = NULL;
	int numTries = 0;  // Keeps track of allocation attempts
	
	// ALLOCATE MEMORY
	while (numTries < HARKLETHREAD_MAX_TRIES && NULL == retVal)
	{
		retVal = (hThrDetails_ptr)calloc(1, sizeof(hThrDetails));
		numTries++;
	}

	// DONE
	return retVal;
}


/*
	PURPOSE - Zeroize, free, and NULL a heap-allocated hCurseWinDetails struct pointer
	INPUT
		oldStruct_ptr - A pointer to a heap-allocated hThrDetails_ptr
	OUTPUT
		On success, true
		On failure, false
	NOTES
		The following struct members will be zeroized, free()d, destroyed, etc:
			name
			threadNum
			threadID(?)
			argvString
			argSize
			pipeMutex
			pipeMutexAttr
			pipeFDs
		The variable pointed at by oldStruct_ptr will be assigned NULL
 */
bool free_a_hThrDetails_ptr(hThrDetails_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	
	// 1. retVal->tName
	// 2. retVal->argvString

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// PTHREAD FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
/////////////////////////// PTHREAD FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
