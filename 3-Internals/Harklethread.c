#include "Harklepipe.h"		// build_a_pipe(), HPIPE_READ, HPIPE_WRITE
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
	int tmpInt = 0;  // Return value from build_a_pipe()/pthread_attr_init()
	
	// INPUT VALIDATION
	if (!start_routine)
	{
		HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, NULL pointer);
		success = false;
	}
	else if ((arg && !argSize) || (!arg && argSize))
	{
		HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, arg/argSize mismatch);
		success = false;
	}
	else if (arg && argSize < 0)
	{
		HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, Invalid argSize);
		success = false;
	}
	
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

	// 2. Initialize the thread attributes
	if (true == success)
	{
		// pthread_attr_t tAttr;  // Attributes used in the creation of a new thread
		tmpInt = pthread_attr_init(&(retVal->tAttr));

		if (tmpInt)
		{
			HARKLE_ERROR(Harklethread, create_a_hThrDetails_ptr, pthread_attr_init failed);
			success = false;
		}
	}
	
	// 3. Initialize the mutex
	if (true == success)
	{
		// 3.1. pthread_mutex_t pipeMutex; // Thread's pipe mutex
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
		// 3.2. pthread_mutexattr_t pipeMutexAttr; // Attributes for thread's pipe mutex
		//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
	}
	
	// 4. Build-A-Pipe Workshop
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


bool free_a_hThrDetails_ptr(hThrDetails_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	hThrDetails_ptr tmpStruct_ptr = NULL;
	bool success = true;  // If anything fails, make this false
	int tmpInt = 0;  // Return values from library functions
	
	// INPUT VALIDATION
	if (!oldStruct_ptr || !(*oldStruct_ptr))
	{
		HARKLE_ERROR(Harklethread, free_a_hThrDetails_ptr, NULL pointer);
		success = false;
	}
	else
	{
		tmpStruct_ptr = *oldStruct_ptr;
	}
	
	// ZEROIZE, FREE, DESTROY STRUCT MEMBERS
	// 1. char* tName;							// Name of the thread (optional)
	if (tmpStruct_ptr->tName)
	{
		if (false == release_a_string(&(tmpStruct_ptr->tName)))
		{
			HARKLE_ERROR(Harklethread, free_a_hThrDetails_ptr, release_a_string failed);
			success = false;
		}
	}
	// 2. int tNum;								// Application-defined number
	tmpStruct_ptr->tNum = 0;
	// 3. pthread_t threadID;					// ID returned by pthread_create()
	//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
	// 4. pthread_attr_t tAttr;					// Attributes used in the creation of a new thread
	if (tmpStruct_ptr->tAttr)
	{
		tmpInt = pthread_attr_destroy(&(tmpStruct_ptr->tAttr));

		if (tmpInt)
		{
			HARKLE_ERROR(Harklethread, free_a_hThrDetails_ptr, pthread_attr_destroy failed);
			success = false;
		}
	}
	// 5. void*(*strtFunc)(void*);				// Function point to the thread's startup function
	tmpStruct_ptr->strtFunc = NULL;
	// 6. void* tArgvString;					// From command-line argument
	if (tmpStruct_ptr->tArgvString)
	{
		if (false == release_a_string_len(&(tmpStruct_ptr->tArgvString), tmpStruct_ptr->tArgSize))
		{
			HARKLE_ERROR(Harklethread, free_a_hThrDetails_ptr, release_a_string_len failed);
			success = false;
		}
	}
	// 7. size_t tArgSize;						// Size of the buffer containing argvString and any nul/NULL termination
	tmpStruct_ptr->tArgSize = 0;
	// 8. pthread_mutex_t pipeMutex; 			// Thread's pipe mutex
	//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
	// 9. pthread_mutexattr_t pipeMutexAttr;	// Attributes for thread's pipe mutex
	//////////////////////////////// IMPLEMENT LATER ////////////////////////////////
	// 10. int pipeFDs[2]; // Pipe used to send data from the thread to the main thread
	tmpStruct_ptr->pipeFDs[HPIPE_READ] = 0;
	tmpStruct_ptr->pipeFDs[HPIPE_WRITE] = 0;

	// DONE
	if (false == success)
	{
		retVal = false;
	}
	
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
