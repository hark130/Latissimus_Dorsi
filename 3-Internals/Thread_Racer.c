/*
		This header was written exclusively to support my 
	3-18_Threading_Practice-1_Grand_Prix.c efforts as part of branch 3-18-1.  
	It may not be useful to any other effort.  I really just wanted to extricate
	what was becoming lengthy "local" code outside of my main.c.
 */

#include "Harklerror.h"				// HARKLE_ERROR
#include "Thread_Racer.h"			// struct threadGrandPrixRace

#ifndef THREADRACER_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define THREADRACER_MAX_TRIES 3
#endif  // THREADRACER_MAX_TRIES

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Allocate heap memory for one threadGrandPrixRace struct
	INPUT - None
	OUPUT
		On success, pointer to a heap-allocated threadGrandPrixRace struct
		On failure, NULL
	NOTES
		It is the caller's responsibility to free() the memory returned
 */
tgpRacer_ptr allocate_tgpRacer_ptr(void)
{
	// LOCAL VARIABLES
	tgpRacer_ptr retVal = NULL;
	int numTries = 0;  // Counter for the number of allocation attempts
	
	// ALLOCATE
	while (numTries < THREADRACER_MAX_TRIES && !retVal)
	{
		retVal = (tgpRacer_ptr)calloc(1, sizeof(tgpRacer));
		numTries++;
	}
	
	if (!retVal)
	{
		HARKLE_ERROR(Thread_Racer, allocate_tgpRacer_ptr, calloc failed);
	}
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Allocate, and populate, heap memory for one threadGrandPrixRace struct
	INPUT
		structDetails - hThrDetails struct containing thread-related details
	OUTPUT
		On success, heap-allocated and populated threadGrandPrixRace struct pointer
		On failure, NULL
	NOTES
		This function calls allocate_tgpRacer_ptr()
		It is the caller's responsibility to free() the memory returned		
 */
tgpRacer_ptr populate_tgpRacer_ptr(hThrDetails_ptr structDetails)
{
	// LOCAL VARIABLES
	tgpRacer_ptr retVal = NULL;
	bool success = true;  // Set this to false if anything fails
	
	// INPUT VALIDATION
	if (!structDetails)
	{
		HARKLE_ERROR(Thread_Racer, allocate_tgpRacer_ptr, NULL pointer);
		success = false;
	}
	
	// ALLOCATION
	if (true == success)
	{
		retVal = allocate_tgpRacer_ptr();
		
		if (!retVal)
		{
			HARKLE_ERROR(Thread_Racer, allocate_tgpRacer_ptr, allocate_tgpRacer_ptr failed);
			success = false;
		}
		else
		{
			retVal->F1Details = structDetails;
		}
	}
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Allocate heap memory for an array of tgpRacer_ptr pointers
	INPUT
		numRacers - Number of entries to allocate in this array
	OUTPUT
		On success, pointer to a heap-allocated array of threadGrandPrixRace 
			truct pointers
		On failure, NULL
	NOTES
		It is the caller's responsibility to free() the memory returned
		The array returned will contain a number of indices equal to
			numRacers + 1 if numRacers is greater than 0
 */
tgpRacer_ptr* allocate_tgpRacer_arr(int numRacers)
{
	// LOCAL VARIABLES
	tgpRacer_ptr* retVal = NULL;
	bool success = true;  // Set this to false if anything fails
	int numTries = 0;  // Counter for the number of allocation attempts
	
	// INPUT VALIDATION
	if (numRacers < 1)
	{
		HARKLE_ERROR(Thread_Racer, allocate_tgpRacer_arr, allocate_tgpRacer_ptr failed);
		success = false;
	}
	
	// ALLOCATION
	if (true == success)
	{
		while (numTries < THREADRACER_MAX_TRIES && !retVal)
		{
			retVal = (tgpRacer_ptr*)calloc(numRacers + 1, sizeof(tgpRacer_ptr));
			numTries++;	
		}
		
		if (!retVal)
		{
			HARKLE_ERROR(Thread_Racer, allocate_tgpRacer_arr, calloc failed);
			success = false;
		}
	}
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Zeroize, free, and NULL a heap-allocated threadGrandPrixRace struct
	INPUT
		oldStruct_ptr - Pointer to a heap-allocated threadGrandPrixRace struct pointer to free
	OUTPUT
		On success, true
		On failure, false
 */
bool free_tgpRacer_ptr(tgpRacer_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	
	// INPUT VALIDATION
	if (!oldStruct_ptr || NULL == *oldStruct_ptr)
	{
		HARKLE_ERROR(Thread_Racer, free_tgpRacer_ptr, NULL pointer);
		retVal = false;
	}
	else
	{
		// Zeroize the struct members		
		// Free the struct members		
		// NULL the struct members
		if (false == free_a_hThrDetails_ptr(&((*oldStruct_ptr)->F1Details)))
		{
			HARKLE_ERROR(Thread_Racer, free_tgpRacer_ptr, free_a_hThrDetails_ptr failed);
			retVal = false;
			(*oldStruct_ptr)->F1Details = NULL;  // Make it NULL anyway
		}
		
		// Free the struct
		free(*oldStruct_ptr);
		
		// NULL the struct
		*oldStruct_ptr = NULL;		
	}	
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Free a heap-allocated array of heap-allocated threadGrandPrixRace
		struct pointers
	INPUT
		oldArr_ptr - Pointer to an array of threadGrandPrixRace struct pointers
	OUTPUT
		On success, true
		On failure, false
 */
bool free_tgpRacer_arr(tgpRacer_ptr** oldArr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __THREAD_RACER__
