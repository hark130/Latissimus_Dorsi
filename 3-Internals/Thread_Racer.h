/*
		This header was written exclusively to support my 
	3-18_Threading_Practice-1_Grand_Prix.c efforts as part of branch 3-18-1.  
	It may not be useful to any other effort.  I really just wanted to extricate
	what was becoming lengthy "local" code outside of my main.c.
 */

#ifndef __THREAD_RACER__
#define __THREAD_RACER__

#include "Harklecurse.h"			// hcCartCoord_ptr
#include "Harklethread.h"			// hThrDetails_ptr
#include <ncurses.h>				// WINDOW
#include <stdbool.h>				// bool, true, false

#define TR_MAX_RACERS 17

typedef struct threadGrandPrixRace
{
	hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
	int trackLen;					// Length of the track
	int numLaps;					// Total number of laps
	int currLap;					// Current lap
	int currPos;					// Current position along the track
	unsigned long relPos;			// Distance behind the leader
	hcCartCoord_ptr currCoord;		// Current cartesian coordinate location
	bool winner;					// This thread won
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
		trkLen - Length of the track this racer has to run
		currentCoord - Optional parameter to specificy the currCoord member
		numberLaps - Number of laps to run
	OUTPUT
		On success, heap-allocated and populated threadGrandPrixRace struct pointer
		On failure, NULL
	NOTES
		This function calls allocate_tgpRacer_ptr()
		It is the caller's responsibility to free() the memory returned		
 */
tgpRacer_ptr populate_tgpRacer_ptr(hThrDetails_ptr structDetails, int trkLen, \
	                               hcCartCoord_ptr currentCoord, int numberLaps);


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
		If the currCoord member exists, this function will not attempt to free it.
			Instead, it will merely set it to NULL.
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// RACER FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Move all the racers around the track based on their progress
	INPUT
		racer_arr - An NULL-terminated array of tgpRacer struct pointers to 
			move around the track
		headNode - First coordinate point on the track, in case the racer makes a lap
		lapNum - Current lap number of the leader
	OUTPUT
		On success, true
		On failure, false
	NOTES
		Calls Thread_Racer::update_racer_pos()
		Will not add racer's that aren't on lapNum
 */
bool update_all_racer_pos(tgpRacer_ptr* racer_arr, hcCartCoord_ptr headNode, int lapNum);


/*
	PURPOSE - Move a racer around the track based on its progress
	INPUT
		racer_ptr - Pointer to a tgpRacer struct representing the racer
			to update
		headNode - First coordinate point on the track, in case the racer makes a lap
		lapNum - Current lap number of the leader
	OUTPUT
		On success, true
		On failure, false
	NOTES
		Calls Thread_Racer::remove_racer_from_track()
		Calls Thread_Racer::add_racer_to_track()
		Will not add racer's that aren't on lapNum`
 */
bool update_racer_pos(tgpRacer_ptr racer_ptr, hcCartCoord_ptr headNode, int lapNum);


/*
	PURPOSE - Remove a racer from its old position on the track
	INPUT
		racer_ptr - Pointer to a tgpRacer struct representing the racer
			to update
	OUTPUT
		On success, true
		On failure, false
	NOTES
		Calls Harklecurse::get_pos_num()
		Calls Thread_Racer::clear_racer_flag()
		Calls Thread_Racer::update_coord_graphic()
 */
bool remove_racer_from_track(tgpRacer_ptr racer_ptr);


/*
	PURPOSE - Remove a racer's flag from a given hcCartesianCoordinate
		struct
	INPUT
		hcCoord - Pointer to a hcCartesianCoordinate struct
		racerNum - Flag to clear
	OUTPUT
		On success, true
		On failure, or if the racer's flag was not present, false
	NOTES
		This function will first verify the racer's flag was raised
			in the first place before trying to clear it
		A racer's flag has a value equal to 2^racerNum
 */
bool clear_racer_flag(hcCartCoord_ptr hcCoord, int racerNum);


/*
	PURPOSE - Add a racer to its new position on the track
	INPUT
		racer_ptr - Pointer to a tgpRacer struct representing the racer
			to update
		headNode - First coordinate point on the track, in case the racer makes a lap
	OUTPUT
		On success, true
		On failure, false
	NOTES
		Calls Harklecurse::get_pos_num()
		Calls Thread_Racer::set_racer_flag()
		Calls Thread_Racer::update_coord_graphic()
 */
bool add_racer_to_track(tgpRacer_ptr racer_ptr, hcCartCoord_ptr headNode);


/*
	PURPOSE - Add a racer's flag to a given hcCartesianCoordinate
		struct
	INPUT
		hcCoord - Pointer to a hcCartesianCoordinate struct
		racerNum - Flag to add
	OUTPUT
		On success, true
		On failure, or if the racer's flag was already present, false
	NOTES
		This function will first verify the racer's flag was not raised
			before trying to clear it
		A racer's flag has a value equal to 2^racerNum
 */
bool set_racer_flag(hcCartCoord_ptr hcCoord, int racerNum);


/*
	PURPOSE - Update a hcCartesianCoordinate struct after some change in
		flag status has been made to it
	INPUT
		hcCoord - Pointer to a hcCartesianCoordinate struct
		newRcrNum - Most recent racer change made to this struct.  It could
			be that newRcrNum just left and its flag has been cleared.  It
			could also mean that this racer just arrived.
	OUTPUT
		On success, true
		On failure, false
	NOTES
		If newRcrNum is not present in the flags, the graphic will be reset
			if it holds newRcrNum's hex digit.  If there are other racer's
			here, it will be set to the lowest racer number.  Otherwise,
			it will be reset to the defGraph.
		If newRcrNum is present in the flags and no other racers are here,
			the graphic will be set to newRcrNum's hex digit.  Otherwise,
			no change.
 */
bool update_coord_graphic(hcCartCoord_ptr hcCoord, int newRcrNum);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// RACER FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GRAND PRIX FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Update the Grand Prix in-race stats window
	INPUT
		rankWin_ptr - Pointer to the winDetails struct containing the rank
			bar information
		racerArr_ptr - An NULL-terminated array of tgpRacer struct pointers
	OUTPUT
		On success, true
		On failure, false
	NOTES
		This function will not call wrefresh().  Instead, it will merely
			update the WINDOW.  The calling function is responsible for
			calling wrefresh().
 */
bool update_ranking_win(winDetails_ptr rankWin_ptr, tgpRacer_ptr* racerArr_ptr);


/*
	PURPOSE - Sort the tgpRacer struct pointers into another array by
		ranking order
	INPUT
		racerArr_ptr - [IN] A NULL-terminated array of tgpRacer struct 
			pointers to sort
		rankedRacer_arr - [OUT] An entirely NULL array of minimum dimension
			TR_MAX_RACERS + 1 into which the tgpRacer struct pointers will
			be sorted
	OUTPUT
		On success, true
		On failure, false
	NOTES
		rankedRacer_arr must be NULL from index 0 to index TR_MAX_RACERS
		I'm currently undecided on whether or not I'll write a helper
			function to also update each racer's relPos member or just
			add the logic into this function
 */
bool sort_racers(tgpRacer_ptr* racerArr_ptr, tgpRacer_ptr* rankedRacer_arr);


/*
	PURPOSE - Determine the lap of the current winner
	INPUT
		racerArr_ptr - [IN] A NULL-terminated array of tgpRacer struct 
			pointers to evaluate
	OUTPUT
		On success, the lap number
		On failure, -1
 */
int highest_lap(tgpRacer_ptr* racerArr_ptr);

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GRAND PRIX FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __THREAD_RACER__
