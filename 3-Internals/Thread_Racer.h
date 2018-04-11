/*
		This header was written exclusively to support my 
	3-18_Threading_Practice-1_Grand_Prix.c efforts as part of branch 3-18-1.  
	It may not be useful to any other effort.  I really just wanted to extricate
	what was becoming lengthy "local" code outside of my main.c.
 */

#ifndef __THREAD_RACER__
#define __THREAD_RACER__

typedef struct threadGrandPrixRace
{
	hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
} tgpRacer, *tgpRacer_ptr;

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
tgpRacer_ptr allocate_tgpRacer_ptr(void);


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
tgpRacer_ptr populate_tgpRacer_ptr(hThrDetails_ptr structDetails);


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
tgpRacer_ptr* allocate_tgpRacer_arr(int numRacers);


/*
	PURPOSE - Zeroize, free, and NULL a heap-allocated threadGrandPrixRace struct
	INPUT
		oldStruct_ptr - Pointer to a heap-allocated threadGrandPrixRace struct pointer to free
	OUTPUT
		On success, true
		On failure, false
	NOTES
		If the F1Details member exists, this function will attempt to call 
			free_a_hThrDetails_ptr() to free() it.
 */
bool free_tgpRacer_ptr(tgpRacer_ptr* oldStruct_ptr);


/*
	PURPOSE - Free a heap-allocated array of heap-allocated threadGrandPrixRace
		struct pointers
	INPUT
		oldArr_ptr - Pointer to an array of threadGrandPrixRace struct pointers
	OUTPUT
		On success, true
		On failure, false
	NOTES
		This function will attempt to free any tgpRacer_ptrs found in the arry
			by calling free_tgpRacer_ptr()
 */
bool free_tgpRacer_arr(tgpRacer_ptr** oldArr_ptr);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __THREAD_RACER__
