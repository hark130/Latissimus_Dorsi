/*
		This header was written exclusively to support my 
	3-18_Threading_Practice-1_Grand_Prix.c efforts as part of branch 3-18-1.  
	It may not be useful to any other effort.  I really just wanted to extricate
	what was becoming lengthy "local" code outside of my main.c.
 */

#include "Harklecurse.h"			// hcCartCoord_ptr
#include "Harklerror.h"				// HARKLE_ERROR
#include "Harklethread.h"			// hThrDetails_ptr
#include <stdlib.h>					// calloc()
#include <string.h>					// strlen()
#include "Thread_Racer.h"			// struct threadGrandPrixRace

#ifndef THREADRACER_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define THREADRACER_MAX_TRIES 3
#endif  // THREADRACER_MAX_TRIES

#define TR_BUFF_SIZE 64  // Default buffer size

// MACRO DEFINING THE RACER FLAGS FOR struct hcCartesianCoordinate.hcFlags
#define TR_RACER_FLAG(n) (n >= 0x0 && 0xF >= n) ? (1 << n) : (0)


/*
	PURPOSE - Convert a number to its single hex character
	INPUT
		num - A number ranging from 0x0 to 0xF, inclusive
	OUTPUT
		On success, the hex character represented by num
		On failure, '\0'
	NOTES
		ithc == integer_to_hex_character
 */
char ithc(int num);


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


tgpRacer_ptr populate_tgpRacer_ptr(hThrDetails_ptr structDetails, int trkLen, hcCartCoord_ptr currentCoord, int numberLaps)
{
	// LOCAL VARIABLES
	tgpRacer_ptr retVal = NULL;
	bool success = true;  // Set this to false if anything fails
	
	// INPUT VALIDATION
	if (!structDetails)
	{
		HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, NULL pointer);
		success = false;
	}
	else if (trkLen < 1)
	{
		HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, Invalid track length);
		success = false;
	}
	else if (numberLaps < 1)
	{
		HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, Invalid number of laps);
		success = false;
	}
	
	// ALLOCATION
	if (true == success)
	{
		retVal = allocate_tgpRacer_ptr();
		
		if (!retVal)
		{
			HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, allocate_tgpRacer_ptr failed);
			success = false;
		}
		else
		{
			// hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
			retVal->F1Details = structDetails;
			// int trackLen;					// Length of the track
			retVal->trackLen = trkLen;
			// int numLaps;						// Total number of laps
			retVal->numLaps = numberLaps;
			// int currLap;						// Current lap
			retVal->currLap = 1;
			// int currPos;						// Current position along the track
			retVal->currPos = 1;  // Starting position
			// unsigned long relPos;			// Distance behind the leader
			retVal->relPos = 0;  // This is set by update_rank_win()
			// hcCartCoord_ptr currCoord;		// Current cartesian coordinate location
			retVal->currCoord = currentCoord;
			// bool winner;						// This thread won
			retVal->winner = false;  // Initialize this

			// Update the currCoord
			if (retVal->currCoord)
			{
				if (false == add_racer_to_track(retVal))
				{
					HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, add_racer_to_track failed);
					success = false;
				}
			}
		}
	}

	// CLEAN UP
	if (false == success)
	{
		if (retVal)
		{
			if (false == free_tgpRacer_ptr(&retVal))
			{
				HARKLE_ERROR(Thread_Racer, populate_tgpRacer_ptr, free_tgpRacer_ptr failed);
			}
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
		// hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
		if ((*oldStruct_ptr)->F1Details)
		{
			if (false == free_a_hThrDetails_ptr(&((*oldStruct_ptr)->F1Details)))
			{
				HARKLE_ERROR(Thread_Racer, free_tgpRacer_ptr, free_a_hThrDetails_ptr failed);
				retVal = false;
				(*oldStruct_ptr)->F1Details = NULL;  // Make it NULL anyway
			}
		}
		// int trackLen;					// Length of the track
		(*oldStruct_ptr)->trackLen = 0;
		// int numLaps;						// Total number of laps
		(*oldStruct_ptr)->numLaps = 0;
		// int currLap;						// Current lap
		(*oldStruct_ptr)->currLap = 0;
		// int currPos;						// Current position along the track
		(*oldStruct_ptr)->currPos = 0;
		// unsigned long relPos;			// Distance behind the leader
		(*oldStruct_ptr)->relPos = 0;
		// hcCartCoord_ptr currCoord;		// Current cartesian coordinate location
		(*oldStruct_ptr)->currCoord = NULL;
		// bool winner;						// This thread won
		(*oldStruct_ptr)->winner = false;
		
		// Free the struct
		free(*oldStruct_ptr);
		
		// NULL the struct
		*oldStruct_ptr = NULL;		
	}	
	
	// DONE
	return retVal;
}


bool free_tgpRacer_arr(tgpRacer_ptr** oldArr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	// tgpRacer_ptr oldStruct = NULL;  // Variable to iterate through the struct pointers
	tgpRacer_ptr* oldArray = NULL;  // Variable to iterate through the array
	
	// INPUT VALIDATION
	if (NULL == oldArr_ptr || NULL == *oldArr_ptr)
	{
		HARKLE_ERROR(Thread_Racer, free_tgpRacer_arr, NULL pointer);
		retVal = false;
	}
	else
	{
		oldArray = *oldArr_ptr;	
	}
	
	// ZEROIZE/FREE/NULL THE STRUCT POINTERS
	while (NULL != *oldArray)
	{
		if (false == free_tgpRacer_ptr(&(*oldArray)))
		{
			HARKLE_ERROR(Thread_Racer, free_tgpRacer_arr, free_tgpRacer_ptr failed);
			retVal = false;
		}
		
		oldArray++;  // Next struct pointer
	}	
	
	// ZEROIZE/FREE/NULL THE ARRAY
	free(*oldArr_ptr);
	*oldArr_ptr = NULL;
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////////// RACER FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


bool update_all_racer_pos(tgpRacer_ptr* racer_arr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	tgpRacer_ptr* tmp_arr = NULL;

	// INPUT VALIDATION
	if (!racer_arr || !(*racer_arr))
	{
		HARKLE_ERROR(Thread_Racer, update_all_racer_pos, NULL pointer);
		retVal = false;
	}

	// ITERATE THROUGH THE RACERS
	if (true == retVal)
	{
		tmp_arr = racer_arr;

		while (*tmp_arr)
		{
			retVal = update_racer_pos(*tmp_arr);
			
			if (false == retVal)
			{
				HARKLE_ERROR(Thread_Racer, update_all_racer_pos, update_racer_pos failed);
				break;
			}
			else
			{
				tmp_arr++;  // Next struct pointer
			}
		}		
	}

	// DONE
	return retVal;
}


bool update_racer_pos(tgpRacer_ptr racer_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;

	// INPUT VALIDATION
	if (!racer_ptr)
	{
		HARKLE_ERROR(Thread_Racer, update_racer_pos, NULL pointer);
		retVal = false;
	}
	else if (!(racer_ptr->currCoord))
	{
		HARKLE_ERROR(Thread_Racer, update_racer_pos, NULL pointer);
		retVal = false;
	}
	else if (racer_ptr->currPos < 0 || racer_ptr->currPos > racer_ptr->trackLen)
	{
		HARKLE_ERROR(Thread_Racer, update_racer_pos, Invalid current position);
		retVal = false;
	}
	else if (racer_ptr->currPos < racer_ptr->currCoord->posNum)
	{
		HARKLE_ERROR(Thread_Racer, update_racer_pos, Racer went backwards);
		retVal = false;
	}
	else if (racer_ptr->F1Details->tNum < 0x0 || racer_ptr->F1Details->tNum > 0xF)
	{
		HARKLE_ERROR(Thread_Racer, update_racer_pos, Invalid racer number);
		retVal = false;
	}
	// else if (racer_ptr->currPos == racer_ptr->trackLen)
	// {
	// 	racer_ptr->winner = true;
	// }
	// Verify racer moved position
	else if (racer_ptr->currPos > racer_ptr->currCoord->posNum)
	{
		// REMOVE RACER FROM THE TRACK
		if (true == retVal)
		{
			retVal = remove_racer_from_track(racer_ptr);

			if (false == retVal)
			{
				HARKLE_ERROR(Thread_Racer, update_racer_pos, remove_racer_from_track failed);
			}
		}

		// ADD RACER TO THE TRACK
		if (true == retVal)
		{
			// fprintf(stdout, "Racer #%d current position is %d and the track length is %d.\n", racer_ptr->F1Details->tNum, racer_ptr->currPos, racer_ptr->trackLen);  // DEBUGGING
			retVal = add_racer_to_track(racer_ptr);

			if (false == retVal)
			{
				HARKLE_ERROR(Thread_Racer, update_racer_pos, add_racer_to_track failed);
			}
		}
	}

	// DONE
	return retVal;
}


bool remove_racer_from_track(tgpRacer_ptr racer_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;

	// INPUT VALIDATION
	if (!racer_ptr)
	{
		HARKLE_ERROR(Thread_Racer, remove_racer_from_track, NULL racer pointer);
		retVal = false;		
	}
	else if (!(racer_ptr->currCoord))
	{
		HARKLE_ERROR(Thread_Racer, remove_racer_from_track, NULL coordinate pointer);
		retVal = false;		
	}
	else if (!(racer_ptr->F1Details))
	{
		HARKLE_ERROR(Thread_Racer, remove_racer_from_track, NULL thread pointer);
		retVal = false;		
	}

	// CLEAR THE RACER'S FLAG
	if (true == retVal)
	{
		retVal = clear_racer_flag(racer_ptr->currCoord, racer_ptr->F1Details->tNum);

		if (false == retVal)
		{
			HARKLE_ERROR(Thread_Racer, remove_racer_from_track, clear_racer_flag failed);
		}
	}

	// UPDATE THE COORDINATE'S GRAPHICS
	if (true == retVal)
	{
		retVal = update_coord_graphic(racer_ptr->currCoord, racer_ptr->F1Details->tNum);

		if (false == retVal)
		{
			HARKLE_ERROR(Thread_Racer, remove_racer_from_track, update_coord_graphic failed);
		}
	}

	// DONE
	return retVal;
}


bool clear_racer_flag(hcCartCoord_ptr hcCoord, int racerNum)
{
	// LOCAL VARIABLES
	bool retVal = true;
	unsigned long racerFlag = 0;  // Store the racer's flag here

	// INPUT VALIDATION
	if (!hcCoord)
	{
		HARKLE_ERROR(Thread_Racer, clear_racer_flag, NULL coordinate pointer);
		retVal = false;	
	}
	else if (racerNum < 0x0 || racerNum > 0xF)
	{
		HARKLE_ERROR(Thread_Racer, clear_racer_flag, Invalid racer number);
		retVal = false;
	}
	else
	{
		racerFlag = TR_RACER_FLAG(racerNum);
	}

	// IS THE FLAG ON?
	if (true == retVal)
	{
		if (racerFlag != (hcCoord->hcFlags & racerFlag))
		{
			HARKLE_ERROR(Thread_Racer, clear_racer_flag, Flag was not set);
			retVal = false;
		}
	}

	// CLEAR THE FLAG
	if (true == retVal)
	{
		hcCoord->hcFlags &= ~racerFlag;
	}

	// DONE
	return retVal;
}


bool add_racer_to_track(tgpRacer_ptr racer_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	hcCartCoord_ptr newCoord = NULL;  // New coord for the racer to 'advance' to

	// INPUT VALIDATION
	if (!racer_ptr)
	{
		HARKLE_ERROR(Thread_Racer, add_racer_to_track, NULL racer pointer);
		retVal = false;		
	}
	else if (!(racer_ptr->currCoord))
	{
		HARKLE_ERROR(Thread_Racer, add_racer_to_track, NULL coordinate pointer);
		retVal = false;		
	}
	else if (!(racer_ptr->F1Details))
	{
		HARKLE_ERROR(Thread_Racer, add_racer_to_track, NULL thread pointer);
		retVal = false;		
	}

	// GET NEW POSITION COORDINATE STRUCT
	if (true == retVal)
	{
		newCoord = get_pos_num(racer_ptr->currCoord, racer_ptr->currPos);

		if (!newCoord)
		{
			fprintf(stderr, "Starting at %d, couldn't find %d\n", racer_ptr->currCoord->posNum, racer_ptr->currPos);  // DEBUGGING
			HARKLE_ERROR(Thread_Racer, add_racer_to_track, get_pos_num failed);
			retVal = false;	
		}
		else
		{
			racer_ptr->currCoord = newCoord;
		}
	}

	// SET THE RACER'S FLAG
	if (true == retVal)
	{
		retVal = set_racer_flag(racer_ptr->currCoord, racer_ptr->F1Details->tNum);

		if (false == retVal)
		{
			HARKLE_ERROR(Thread_Racer, add_racer_to_track, clear_racer_flag failed);
		}
	}

	// UPDATE THE COORDINATE'S GRAPHICS
	if (true == retVal)
	{
		retVal = update_coord_graphic(racer_ptr->currCoord, racer_ptr->F1Details->tNum);

		if (false == retVal)
		{
			HARKLE_ERROR(Thread_Racer, add_racer_to_track, update_coord_graphic failed);
		}
	}

	// DONE
	return retVal;
}


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
bool set_racer_flag(hcCartCoord_ptr hcCoord, int racerNum)
{
	// LOCAL VARIABLES
	bool retVal = true;
	unsigned long racerFlag = 0;  // Store the racer's flag here

	// INPUT VALIDATION
	if (!hcCoord)
	{
		HARKLE_ERROR(Thread_Racer, clear_racer_flag, NULL coordinate pointer);
		retVal = false;	
	}
	else if (racerNum < 0x0 || racerNum > 0xF)
	{
		HARKLE_ERROR(Thread_Racer, clear_racer_flag, Invalid racer number);
		retVal = false;
	}
	else
	{
		racerFlag = TR_RACER_FLAG(racerNum);
	}

	// IS THE FLAG OFF?
	if (true == retVal)
	{
		if (0 != (hcCoord->hcFlags & racerFlag))
		{
			HARKLE_ERROR(Thread_Racer, clear_racer_flag, Flag was set);
			retVal = false;
		}
	}

	// SET THE FLAG
	if (true == retVal)
	{
		hcCoord->hcFlags |= racerFlag;
	}

	// DONE
	return retVal;
}


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
bool update_coord_graphic(hcCartCoord_ptr hcCoord, int newRcrNum)
{
	// LOCAL VARIABLES
	bool retVal = true;
	unsigned long racerFlag = 0;  // Store the racer's flag here
	char newRcrHex = 0;  // Store the racer's hex digit here
	int i = 0;  // Iterating variable

	// INPUT VALIDATION
	if (!hcCoord)
	{
		HARKLE_ERROR(Thread_Racer, update_coord_graphic, NULL pointer);
		retVal = false;
	}
	else if (newRcrNum < 0x0 || newRcrNum > 0xF)
	{
		HARKLE_ERROR(Thread_Racer, update_coord_graphic, Invalid racer number);
		retVal = false;
	}
	else
	{
		racerFlag = TR_RACER_FLAG(newRcrNum);
		newRcrHex = ithc(newRcrNum);

		if (!newRcrHex)
		{
			HARKLE_ERROR(Thread_Racer, update_coord_graphic, ithc failed);
			retVal = false;
		}
	}

	// UPDATE THE COORDINATE POINT
	if (true == retVal)
	{
		// If newRcrNum is not present in the flags...
		if (racerFlag != (hcCoord->hcFlags & racerFlag))
		{
			// If newRcrNum used to be here...
			if (hcCoord->graphic == newRcrHex)
			{
				// Reset the graphic to defGraph
				hcCoord->graphic = hcCoord->defGraph;

				// If there's another racer here...
				for (i = 0x0; i <= 0xF; i++)
				{
					if (i != newRcrNum)
					{
						racerFlag = TR_RACER_FLAG(i);  // Get the flag

						if (racerFlag == (hcCoord->hcFlags & racerFlag))
						{
							newRcrHex = ithc(i);

							if (!newRcrHex)
							{
								HARKLE_ERROR(Thread_Racer, update_coord_graphic, ithc failed);
								retVal = false;
							}
							else
							{
								// ...set that racer's hex digit
								hcCoord->graphic = newRcrHex;
								break;
							}
						}
					}
				}
			}
		}
		// If newRcrNum is present in the flags...
		else
		{
			// If no one else is here...
			if (racerFlag == hcCoord->hcFlags && hcCoord->graphic == hcCoord->defGraph)
			{
				// ...set the new racer's digit
				hcCoord->graphic = newRcrHex;
			}
			// Otherwise, don't change anything because the new racer wasn't here first
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// RACER FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GRAND PRIX FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Update the Grand Prix in-race stats window
	INPUT
		rankWin_ptr - Pointer to the WINDOW object holding the rank bar
		racerArr_ptr - An NULL-terminated array of tgpRacer struct pointers
	OUTPUT
		On success, true
		On failure, false
	NOTES
		This function will not call wrefresh().  Instead, it will merely
			update the WINDOW.  The calling function is responsible for
			calling wrefresh().
 */
bool update_ranking_win(winDetails_ptr rankWin_ptr, tgpRacer_ptr* racerArr_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	WINDOW* tWin_ptr = NULL;  // Get this from rankWin_ptr for ease of reference
	int maxWid = 0;  // Max printable window width
	int maxLen = 0;  // Max printable window length
	char tmpString1[TR_BUFF_SIZE + 1] = { 0 };  // Temp array to prep printed lines
	char tmpString2[TR_BUFF_SIZE + 1] = { 0 };  // Temp array to prep printed lines
	int tmpInt = 0;  // Temp int for calculations
	tgpRacer_ptr rankedRacerArr[TR_MAX_RACERS + 1] = { NULL };  // Holds the ranked racers
	int i = 0;  // Iterating variable
	int topLap = 0;  // Highest lap number among the racers
	int totalLaps = 0;  // Total laps
	int topPosition = 0;  // Laps * currPos for the racer in 1st place
	int currPrintRow = 1;  // Keep track of which row is currently being printed
	char currentRacer = 0;  // Store the current racer's 'symbol' (hex digit) here


	// INPUT VALIDATION
	if (!rankWin_ptr)
	{
		HARKLE_ERROR(Thread_Racer, update_ranking_win, NULL struct pointer);
		retVal = false;
	}
	else if (!racerArr_ptr || !(*racerArr_ptr))
	{
		HARKLE_ERROR(Thread_Racer, update_ranking_win, NULL racer pointer);
		retVal = false;
	}
	//////////////////////////// UPDATE THIS LATER ///////////////////////////
	else if (rankWin_ptr->nCols < 39)
	{
		HARKLE_ERROR(Thread_Racer, update_ranking_win, Invalid number of columns);
		retVal = false;
	}
	//////////////////////////// UPDATE THIS LATER ///////////////////////////
	else if (rankWin_ptr->nRows < 5)
	{
		HARKLE_ERROR(Thread_Racer, update_ranking_win, Invalid number of rows);
		retVal = false;
	}
	else if (!(rankWin_ptr->win_ptr))
	{
		HARKLE_ERROR(Thread_Racer, update_ranking_win, NULL WINDOW pointer);
		retVal = false;
	}
	else
	{
		tWin_ptr = rankWin_ptr->win_ptr;
	}

	if (true == retVal)
	{
		// GET WINDOW SIZE
		if (true == retVal)
		{
			maxWid = rankWin_ptr->nCols - 4;
			maxLen = rankWin_ptr->nRows - 4;
			// fprintf(stdout, "maxWid == %d - strlen(%lu) == %d\n", maxWid, strlen("THREAD GRAND PRIX"), tmpInt);  // DEBUGGING
		}

		// RANK RACERS
		// for (i = 0; i < TR_MAX_RACERS; i++)
		// {
		// 	if (NULL == racerArr_ptr[i])
		// 	{
		// 		break;
		// 	}
		// 	else
		// 	{
		// 		rankedRacerArr[i] = racerArr_ptr[i];
		// 	}
		// }
		if (false == sort_racers(racerArr_ptr, rankedRacerArr))
		{
			HARKLE_ERROR(Thread_Racer, update_ranking_win, sort_racers failed);
			retVal = false;
		}
		else
		{
			// Get top lap
			topLap = rankedRacerArr[0]->currLap;
			// Get total laps
			totalLaps = racerArr_ptr[0]->numLaps;
		}
	}

	// FORMAT ROWS
	// 1. Header Row - THREAD GRAND PRIX
	if (true == retVal)
	{
		// 1.1. Setup temp buffer
		snprintf(tmpString1, TR_BUFF_SIZE, "%s", "THREAD GRAND PRIX");
		// 1.2. Determine size of this line
		tmpInt = (maxWid - strlen(tmpString1)) / 2;
		// 1.3. Print the buffer within the spacing
		if (true == retVal && OK != mvwprintw(tWin_ptr, currPrintRow, 2, "%*s", tmpInt + strlen(tmpString1), tmpString1))
		{
			HARKLE_ERROR(Thread_Racer, update_ranking_win, mvwprintw failed on the header);
			retVal = false;
		}
	}

	// 2. Race Details - LAP           X of Y
	if (true == retVal)
	{
		// 2.0. Increment print row
		currPrintRow++;
		// 2.1. Setup temp buffers
		snprintf(tmpString1, TR_BUFF_SIZE, "%s", "LAP");
		snprintf(tmpString2, TR_BUFF_SIZE, "%d of %d", topLap, totalLaps);
		// 2.2. Determine size of this line
		tmpInt = maxWid - strlen(tmpString1) - strlen(tmpString2);
		if (tmpInt < 0)
		{
			HARKLE_ERROR(Thread_Racer, update_ranking_win, Lap line is too long);
			retVal = false;
		}
		else if (true == retVal && OK != mvwprintw(tWin_ptr, currPrintRow, 2, \
			                                       "%s%*s", tmpString1, tmpInt + \
			                                       strlen(tmpString2), tmpString2))
		{
			HARKLE_ERROR(Thread_Racer, update_ranking_win, mvwprintw failed on the header);
			retVal = false;
		}
	}

	// 3. Ranked Racers
	if (true == retVal)
	{
		for (i = 0; i < TR_MAX_RACERS; i++)
		{
			// 3.0. Verify the bounds
			// 3.0.1. Check number of racers
			if (NULL == rankedRacerArr[i])
			{
				break;  // No more racers
			}
			// 3.0.2. Verify we're not going off the bottom
			else if (currPrintRow == maxLen)
			{
				break;  // No more room
			}
			else
			{
				// fprintf(stdout, "\nPrinting Thread #%d\n", rankedRacerArr[i]->F1Details->tNum);  // DEBUGGING
				currPrintRow++;  // Print another racer
				currentRacer = ithc(rankedRacerArr[i]->F1Details->tNum);

				if (!currentRacer)
				{
					HARKLE_ERROR(Thread_Racer, update_ranking_win, ithc failed);
					retVal = false;
					break;
				}
			}

			// 3.1. Setup temp buffers
			if (rankedRacerArr[i]->F1Details->tName)
			{
				snprintf(tmpString1, TR_BUFF_SIZE, "%2d (0x%c) %20s -%4lu", \
					     i + 1, currentRacer, rankedRacerArr[i]->F1Details->tName, \
					     rankedRacerArr[i]->relPos);
			}
			else
			{
				snprintf(tmpString2, TR_BUFF_SIZE, "%s%02d", "Thread #", \
					     rankedRacerArr[i]->F1Details->tNum);
				if (i > 0)
				{
					snprintf(tmpString1, TR_BUFF_SIZE, "%2d (0x%c) %-20s -%04lu", \
						     i + 1, currentRacer, tmpString2, \
						     rankedRacerArr[i]->relPos);
				}
				else
				{
					snprintf(tmpString1, TR_BUFF_SIZE, "%2d (0x%c) %-26s", \
						     i + 1, currentRacer, tmpString2);
				}
			}

			// 3.2. Print the line
			if (OK != mvwprintw(tWin_ptr, currPrintRow, 2, \
				                "%*s", maxWid, tmpString1))
			{
				HARKLE_ERROR(Thread_Racer, update_ranking_win, mvwprintw failed on a racer);
				retVal = false;
			}
		}		
	}

	// DONE
	return retVal;
}


bool sort_racers(tgpRacer_ptr* racerArr_ptr, tgpRacer_ptr* rankedRacer_arr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails
	int i = 0;  // Iterating variable
	// int j = 0;  // Iterating variable
	tgpRacer_ptr realLeader = NULL;  // First place
	tgpRacer_ptr currLeader = NULL;  // Temporary 'benchmark' to check against
	// tgpRacer_ptr* dest_arr = NULL;  // Iterating pointer for rankedRacer_arr
	// Set an index to nul if a racerArr_ptr index number has already been
	//	sorted into rankedRacer_arr
	char bookKeep[TR_MAX_RACERS + 1] = { 0 };  // Bookkeeping for the sorting
	char bookChar = 'X';  // Bookkeeping char
	int currLdrIndex = 0;  // Keep track of the index for the current leader
	int numRacersRanked = 0;  // The number of racers that have already been sorted
	int totalRacers = 0;  // Number of pointers in racerArr_ptr

	// INPUT VALIDATION
	if (!racerArr_ptr || !(*racerArr_ptr))
	{
		HARKLE_ERROR(Thread_Racer, sort_racers, NULL racer array pointer);
		retVal = false;
	}
	else if (!rankedRacer_arr)
	{
		HARKLE_ERROR(Thread_Racer, sort_racers, NULL ranked racer array pointer);
		retVal = false;
	}
	else
	{
		// Verify racerArr_ptr doesn't exceed TR_MAX_RACERS
		for (i = 0; i <= TR_MAX_RACERS; i++)
		{
			if (racerArr_ptr[i])
			{
				totalRacers++;
			}
			else
			{
				break;  // Found the NULL termination
			}
		}

		if (totalRacers > TR_MAX_RACERS)
		{
			HARKLE_ERROR(Thread_Racer, sort_racers, Too many racers);
			retVal = false;
		}
		else if (totalRacers < 1)
		{
			HARKLE_ERROR(Thread_Racer, sort_racers, Too few racers);
			retVal = false;
		}

		// Verify rankedRacer_arr is emtpy
		for (i = 0; i <= TR_MAX_RACERS; i++)
		{
			if (NULL != rankedRacer_arr[i])
			{
				HARKLE_ERROR(Thread_Racer, sort_racers, Ranked racer array contains a pointer);
				retVal = false;
				break;
			}
		}
	}

	// PREPARE THE BOOKKEEPING ARRAY
	if (true == retVal)
	{
		for (i = 0; i < totalRacers; i++)
		{
			bookKeep[i] = bookChar;
		}
	}

	// SORT THE RACERS
	if (true == retVal)
	{
		// 0. Prepare the destination pointer
		// dest_arr = rankedRacer_arr;

		// 1. Find first place racer
		while (true == retVal && numRacersRanked < totalRacers)
		{
			// Find the current leader in the race
			for (i = 0; i < totalRacers; i++)
			{
				// End of racers?
				if (NULL == racerArr_ptr[i])
				{
					break;
				}
				// Did we already sort this one?
				else if (!bookKeep[i])
				{
					continue;
				}
				// Find a racer that's ahead of the current leader
				else
				{
					// Haven't found one yet
					if (!currLeader)
					{
						// You're the leader now!
						currLeader = racerArr_ptr[i];
						currLdrIndex = i;
					}
					// Ahead on laps
					else if (racerArr_ptr[i]->currLap > currLeader->currLap)
					{
						currLeader = racerArr_ptr[i];
						currLdrIndex = i;
					}
					// Same lap but ahead on position
					else if (racerArr_ptr[i]->currLap == currLeader->currLap && \
						     racerArr_ptr[i]->currPos > currLeader->currPos)
					{
						currLeader = racerArr_ptr[i];
						currLdrIndex = i;
					}
				}
			}

			// Did we find a current leader?
			if (currLeader)
			{
				if (0 == numRacersRanked)
				{
					realLeader = currLeader;  // Store the current leader
					realLeader->relPos = 0;  // Relative position is front of the pack
				}
				else
				{
					// UPDATE RACERS RELATIVE POSITION
					currLeader->relPos = ((realLeader->currLap * realLeader->trackLen) + realLeader->currPos) - \
					                     ((currLeader->currLap * currLeader->trackLen) + currLeader->currPos);
				}
				// Store this racer in the destination array
				// fprintf(stdout, "Just sorted Thread #%d\n", currLeader->F1Details->tNum);  // DEBUGGING
				(*(rankedRacer_arr + numRacersRanked)) = currLeader;
				numRacersRanked++;  // Advance the iterating variable to the next index
				bookKeep[currLdrIndex] = 0;  // Clear the book keeping char for this racer
				currLeader = NULL;  // Reset temp variable
			}
			// How did we get here?!
			else
			{
				HARKLE_ERROR(Thread_Racer, sort_racers, First place racer sorting logic error);
				retVal = false;
			}
		}
	}

	// DONE
	return retVal;
}

//////////////////////////////////////////////////////////////////////////////
////////////////////////// GRAND PRIX FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Convert a number to its single hex character
	INPUT
		num - A number ranging from 0x0 to 0xF, inclusive
	OUTPUT
		On success, the hex character represented by num
		On failure, '\0'
	NOTES
		ithc == integer_to_hex_character
 */
char ithc(int num)
{
	// LOCAL VARIABLES
	char retVal = '\0';
	char translate[] = { "0123456789ABCDEF" };  // Index-to-hex conversion

	// INPUT VALIDATION
	if (num >= 0x0 && 0xF >= num)
	{
		retVal = translate[num];
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////// LOCAL HELPER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////
