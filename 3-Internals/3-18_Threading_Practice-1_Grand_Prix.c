/*
  The intent of this project is to 'race' threads while displaying their near-real-time status on a ncurses-drawn race track.
  
// DESIGN
1. Visual Design
	1. Racetrack
		3. "Car" indicators
			* Color circles
			* Color numbers
			* Current winner bolded
		4. Color
			* White
			* Bold
			* Dynamic?
	2. Rank Bar
		3. Data displayed
			* Green plus/red minus indicating change in status?
			* Thread name
			* Thread ranking
			* Thread lap
	3. Final Results
		1. Data displayed
			* Thread name
			* Thread lap at end
			* Thread total time
2. Program Design
	1. Main
		1. Support for CLI args
			* -n Number of threads (10)
			* -l Number of laps (1)
			* -u Update interval (1 sec)
		2. Determine winner
			* First to exit()?
			* Photo finish (time)?
			
// TASKS
[ ] Set max number of racers to 0xF
[X] Thread function
	[X] Calculate something
	[X] Check mutex and wait
	[X] Update pipe
	[X] Loop
[X] Main thread function
	[X] Sleep for an update interval
	[X] Lock all the mutexes
	[X] Read all the pipes
	[X] Update the race
	[X] Unlock all the pipes
	[X] Loop
[X] Harklethread.h (Thread Library)
[X] Harklepipe.h (Pipe Library)
[ ] Mini map
	[X] Draw it
	[X] Dynamically size it
	[ ] Color it
	[ ] Indicators
		[ ] Place it
		[ ] Dynamically place it (on track or off track)
		[ ] Color it
[ ] Rank Bar
	[X] Draw it
	[X] Dynamically size it
	[ ] Populate it
		[ ] Thread names
		[ ] Thread colors
		[ ] Thread details
			[ ] Thread placing
			[ ] Thread name
			[ ] Thread lap
			[ ] Iterations behind leader
[ ] Final Results
	[ ] Draw it
	[ ] Dynamically size it
	[ ] Populate it
		[ ] Thread names
		[ ] Thread colors
		[ ] Thread details
			[ ] Thread placing
			[ ] Thread total time

// TO DO 
BUGS
	[ ] Oddly sized windows (e.g., Taller than it is wide)
	[ ] Verify that the populate_whatever() function validates the tNum as < 0xF for the purposes of printing/formatting
	[ ] Refactor the way thread names get randomized.  Keep track of the remaining names, rando(1, numRemNames), iterate
		through the remaining names to get that name (skipping over, and not counting, NULL).  It's a more efficient way
		of approaching the problem.
	[ ] Maybe lapNum isn't needed any longer as a new argument to some of the update_racer_*() functions
	[ ] LAST!  Run grand_prix.exe through redirect_bin_output.exe to look for otherwise 'silenced' errors

IDEAS
	[ ] If the rankWin nRows are enough, added some horizontal border above and below the laps line
	[ ] Color code the racers according to ROY-G-BIV
	[ ] Bold the titles
	[ ] Add support for CLI
 */

#include <errno.h>
#include "Harklecurse.h"		// kill_a_window()
#include "Harklepipe.h"			// read_a_pipe()
#include "Harklemath.h"			// determine_center(), NUM_PRIMES_ULLONG
#include "Harklerror.h"			// HARKLE_ERROR()
#include "Harklethread.h"
#include <limits.h>				// ULLONG_MAX
#include <math.h>				// sqrt()
#include "Memoroad.h"
#include <ncurses.h>			// initscr(), refresh(), endwin()
#include "Rando.h"				// rando_a_uint()
#include <string.h>				// memset()
#include <stdbool.h>			// bool, true, false
#include <stdint.h>				// intptr_t
#include <stdio.h>				// printf
#include <stdlib.h>				// calloc()
#include "Thread_Racer.h"
#include <unistd.h>				// sleep()

#define OUTER_BORDER_WIDTH_H 4
#define OUTER_BORDER_WIDTH_V 2
#define INNER_BORDER_WIDTH_H 4
#define INNER_BORDER_WIDTH_V 2
#define RANK_BAR_WIDTH 40

#ifndef GRAND_PRIX_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define GRAND_PRIX_MAX_TRIES 3
#endif // GRAND_PRIX_MAX_TRIES

#define SLEEPY_OFFICIALS .5	// Number of seconds for the main thread to sleep each evaluation
#define SLEEPY_RACER 0  	// Number of seconds for racer_sleepy_func() to sleep
#define FAST_RACER 10000		// Multiple to increase the number of calculations, minimum 1
#define SLEEPY_BUFF 20  	// Local buffer size

// typedef struct threadGrandPrixRace
// {
// 	hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
// } tgpRacer, *tgpRacer_ptr;


/*
	PURPOSE - Function to be executed by each of the racer's
	INPUT
		racerNum - The racer thread's tNum
	OUTPUT - None
	NOTES
		This function is basically a placeholder until most everything
			else is constructed.
 */
void racer_func(int racerNum);


/*
	PURPOSE - Determine if an unsigned long long is prime
	INPUT
		num - An unsigned long long to test as prime
	OUTPUT
		If prime, true
		If not, false
 */
bool is_this_prime(unsigned long long num);


/*
	PURPOSE - This racer function will attempt to locate trackLen number
		of primes starting at ULLONG_MAX and iterating down
	INPUT
		F1Details - A hThrDetails struct pointer with details about this
			thread.  Most importantly, the pipe with which it needs to
			communicate back to the main thread.
		trackLen - The number of primes to calculate.  The main thread
			will likely pass in the number of "coordinates" calculated
			for the track unless that number is larger than 
			NUM_PRIMES_ULLONG (which is the approximate number
			of primes less than ULLONG_MAX according to the prime number
			theorem)
	OUTPUT - None
 */
void racer_ull_prime_func(hThrDetails_ptr F1Details, unsigned long long trackLen);
void racer_ul_prime_func(hThrDetails_ptr F1Details, unsigned long trackLen);
void racer_uint_prime_func(hThrDetails_ptr F1Details, unsigned int trackLen);


/*
	PURPOSE - This racer function will merely increment and sleep()
	INPUT
		F1Details - A hThrDetails struct pointer with details about this
			thread.  Most importantly, the pipe with which it needs to
			communicate back to the main thread.
		threadDets - tgpRacer struct pointer with all the details
	OUTPUT - None		
 */
void racer_sleepy_func(tgpRacer_ptr threadDets);
// void racer_sleepy_func(int trackLen);

// Randomize a uint and test for prime.  Send current count to the pipe.
void racer_rando_prime(tgpRacer_ptr threadDets);


int main(int argc, char** argv)
{
	// LOCAL VARIABLES
	//////////////////////////// Program Defaults ////////////////////////////
	int numF1s = 10;  // Number of 'racing' threads to be spawned
	int raceLen = 162;  // Length of the race in miles, rounded up
	int upInterval = 1;  // Frequency of main thread updates
	int numLaps = 78;  // Number of the laps the 'racing' threads must take
	// int numLaps = 2;  // Number of the laps the 'racing' threads must take
	//////////////////////////////////////////////////////////////////////////
	int retVal = 0;  // Function's return value, also holds ncurses return values
	int errNum = 0;  // Holds errno returned from read_a_pipe()
	bool foundWinner = false;  // Update to true if any thread wins
	bool success = true;  // Make this false if anything fails
	winDetails_ptr stdWin = NULL;  // hCurseWinDetails struct pointer for the stdscr window
	winDetails_ptr trackWin = NULL;  // hCurseWinDetails struct pointer for the track window
	winDetails_ptr rankBarWin = NULL;  // hCurseWinDetails struct pointer for the rank bar window
	int numCols = 0;  // Number of columns available
	int numRows = 0;  // Number of rows available
	int i = 0;  // Iterating variable
	int j = 0;  // Iterating variable
	int numTries = 0;  // Counter for memory allocation function calls
	int tmpInt = 0;  // Holds various return values
	
	// Race Cars
	tgpRacer_ptr* racerArr_ptr = NULL;  // Array of racer struct pointers
	tgpRacer_ptr racer_ptr = NULL;  // Index from the array of racer struct pointers
	hThrDetails_ptr tmpMember = NULL;  // Temp variable to hold the F1Details during creation
	char* pipeReads = NULL;  // Returned values from read_a_pipe() calls
	char* racerNames[] = {
		"L. Torvalds", "H. Sweeten", "G. Uytterhoeven", "A. Bergmann", "A. Viro", "T. Iwai", \
		"L. Clausen", "M. Chehab", "V. Syrjala", "L. Walleij", "D. Carpenter", "Intel", \
		"Red Hat", "Linaro", "Samsung", "SUSE", "IBM", "Renesas Electronics", \
	};
	char* tmpRacerName = NULL;  // Temp racer name to assign
	int tmpNameIndex = 0;  // Temp variable to hold a randomized index into racerNames
	size_t numNames = sizeof(racerNames) / sizeof(*racerNames);  // Dimension of racerNames
	int highLap = 0;  // Current lap of the front runner

	// Race Track
	int internalBuffer = 0;  // Distance between ellipse's V1, V2, V3, V4 and the trackWin border
	double* trackPntArray = NULL;  // Return value from plot_ellipse_points()
	int numTrackPnts = 0;  // Number of track points in trackPntArray
	int cntTrkPntX = 0;  // x coordinate for the center of the track window
	int cntTrkPntY = 0;  // y coordinate for the center of the track window
	int axisLenX = 0;  // Calculate the maximum value of the ellipse function's variable "a" here
	int axisLenY = 0;  // Calculate the maximum value of the ellipse function's variable "b" here
	hcCartCoord_ptr trkHeadNode = NULL;  // Head node of the linked list of track plot points
	hcCartCoord_ptr newNode = NULL;  // Newly created node is held here prior to linking
	hcCartCoord_ptr tmpNode = NULL;  // Holds the return value from function calls
	
	// INPUT VALIDATION
	if (numF1s < 1)
	{
		HARKLE_ERROR(Grand_Prix, main, Invalid parameter);
		success = false;
	}

	// PRINT THE TRACK
	if (true == success)
	{
		// 1. Setup ncurses
		initscr();  // Start curses mode
		cbreak();  // Disables line buffering and erase/kill character-processing
		// raw();  // Line buffering disabled
		noecho();  // Disable echo

		// 2. Main Window (stdscr)
		stdWin = build_a_winDetails_ptr();

		if (!stdWin)
		{
			HARKLE_ERROR(Grand_Prix, main, build_a_winDetails_ptr failed);
			success = false;
		}
		else
		{
			// Populate Main Window struct
			stdWin->win_ptr = stdscr;
			stdWin->upperR = 0;
			stdWin->leftC = 0;
			getmaxyx(stdscr, stdWin->nRows, stdWin->nCols);  // Determine the maximum dimensions
			if (ERR == stdWin->nRows || ERR == stdWin->nCols)
			{
				HARKLE_ERROR(Grand_Prix, main, getmaxyx failed);
				success = false;
			}

			retVal = wborder(stdWin->win_ptr, \
				             ACS_VLINE, ACS_VLINE, \
			    	         ACS_HLINE, ACS_HLINE, \
			        	     ACS_ULCORNER, ACS_URCORNER, \
			            	 ACS_LLCORNER, ACS_LRCORNER);

			if (OK != retVal)
			{
				HARKLE_ERROR(Grand_Prix, main, wborder failed);
				success = false;
			}
		}
	}

	// 3. Track Window
	if (true == success)
	{
		trackWin = \
			populate_a_winDetails_ptr(stdWin->nRows - INNER_BORDER_WIDTH_V - OUTER_BORDER_WIDTH_V, \
			                          stdWin->nCols - RANK_BAR_WIDTH - INNER_BORDER_WIDTH_H - (2 * OUTER_BORDER_WIDTH_H), \
			                          OUTER_BORDER_WIDTH_V, \
			                          OUTER_BORDER_WIDTH_H);

		if (!trackWin)
		{
			HARKLE_ERROR(Grand_Prix, main, populate_a_winDetails_ptr failed);
			success = false;
		}
		else
		{
			retVal = wborder(trackWin->win_ptr, \
				             ACS_VLINE, ACS_VLINE, \
			    	         ACS_HLINE, ACS_HLINE, \
			        	     ACS_ULCORNER, ACS_URCORNER, \
			            	 ACS_LLCORNER, ACS_LRCORNER);

			if (OK != retVal)
			{
				HARKLE_ERROR(Grand_Prix, main, wborder failed);
				success = false;
			}
		}
	}
		
	// 4. Rank Bar Window
	if (true == success)
	{
		rankBarWin = \
			populate_a_winDetails_ptr(stdWin->nRows - (2 * OUTER_BORDER_WIDTH_V), \
			                          RANK_BAR_WIDTH, \
			                          OUTER_BORDER_WIDTH_V, \
			                          trackWin->nCols + (2 * OUTER_BORDER_WIDTH_H));

		if (!rankBarWin)
		{
			HARKLE_ERROR(Grand_Prix, main, populate_a_winDetails_ptr failed);
			success = false;
		}
		else
		{
			retVal = wborder(rankBarWin->win_ptr, \
				             ACS_VLINE, ACS_VLINE, \
			    	         ACS_HLINE, ACS_HLINE, \
			        	     ACS_ULCORNER, ACS_URCORNER, \
			            	 ACS_LLCORNER, ACS_LRCORNER);

			if (OK != retVal)
			{
				HARKLE_ERROR(Grand_Prix, main, wborder failed);
				success = false;
			}
		}
	}

	// 5. Race Track
	if (true == success)
	{
		// 5.1. Get the center of the track window
		if (false == determine_center(trackWin->nCols, trackWin->nRows, &cntTrkPntX, &cntTrkPntY, HM_UP_LEFT))
		{
			HARKLE_ERROR(Grand_Prix, main, determine_center failed);
			success = false;
		}
		else
		{
			// 5.2. Calculate the axis of the ellipse based on the window size

			// Horizontal axis
			if (trackWin->nCols & 1)  // Windown width is odd
			{
				axisLenX = (trackWin->nCols - 1) / 2;
			}
			else
			{
				axisLenX = (trackWin->nCols - 2) / 2;
			}

			// Vertical axis
			if (trackWin->nRows & 1)
			{
				axisLenY = (trackWin->nRows - 1) / 2;
			}
			else
			{
				axisLenY = (trackWin->nRows - 2) / 2;
			}
			
			// Calculate the ellipse buffer
			internalBuffer = (int)trackWin->nCols / 50;
			if (internalBuffer < 1)
			{
				internalBuffer = 1;	
			}
			
			// Reduce the horizontal/vertical axis by the buffer
			axisLenX -= internalBuffer;
			axisLenY -= internalBuffer;
			
			// fprintf(stdout, "Win size == width: %d\tlength == %d\n", trackWin->nCols, trackWin->nRows);  // DEBUGGING
			// fprintf(stdout, "'a' (Horizontal) == %d\t'b' (Vertical) == %d\n", axisLenX, axisLenY);  // DEBUGGING
			// fprintf(stdout, "Internal buffer == %d\n", internalBuffer);  // DEBUGGING
			
			// 5.3. Get plot points for the race track
			trackPntArray = plot_ellipse_points(axisLenX, axisLenY, &numTrackPnts);
			// trackPntArray = plot_ellipse_points(trackWin->nCols, trackWin->nRows, &numTrackPnts);

			if (NULL == trackPntArray || 0 == numTrackPnts || 0 != numTrackPnts % 2)
			{
				HARKLE_ERROR(Grand_Prix, main, plot_ellipse_points failed);
				success = false;
			}
			else
			{
				// 5.3. Convert plot points into linked list of Cartesian Coordinates
				trkHeadNode = build_geometric_list(trackPntArray, numTrackPnts, cntTrkPntX, cntTrkPntY);

				if (NULL == trkHeadNode)
				{
					HARKLE_ERROR(Grand_Prix, main, build_geometric_list failed);
					success = false;
				}
				else
				{
					// 5.4. Number all the Cartesian Coordinates in the linked list
					if (numTrackPnts / 2 != number_cartCoord_nodes(trkHeadNode))
					{
						// fprintf(stdout, "Track length == %d\tNumber Nodes == %d\n", numTrackPnts, number_cartCoord_nodes(trkHeadNode));  // DEBUGGING
						HARKLE_ERROR(Grand_Prix, main, number_cartCoord_nodes failed);
						success = false;
					}
				}
			}
		}
	}

	// 5.4. Print Track Points
	if (true == success)
	{
		if (false == print_plot_list(trackWin->win_ptr, trkHeadNode))
		{
			HARKLE_ERROR(Grand_Prix, main, print_plot_list failed);
			success = false;
		}
		// else
		// {
		// 	refresh();  // Print it on the real screen
		// }
	}

	// BUILD RACE CARS
	if (true == success)
	{
		// 1. Allocate an array for the racers
		racerArr_ptr = allocate_tgpRacer_arr(numF1s);
		// while (numTries < GRAND_PRIX_MAX_TRIES && NULL == racerArr_ptr)
		// {
		// 	racerArr_ptr = (tgpRacer_ptr*)calloc(numF1s, sizeof(tgpRacer_ptr));
		// 	numTries++;
		// }

		if (NULL == racerArr_ptr)
		{
			HARKLE_ERROR(Grand_Prix, main, allocate_tgpRacer_arr failed);
			success = false;
		}
		else
		{
			// 2. Create the racers
			for (i = 0; i < numF1s; i++)
			{
				// 2.0. Randomize a name for the racer
				do
				{
					tmpNameIndex = rando_me(0, numNames);
					
					if (tmpNameIndex < 0 || tmpNameIndex > numNames)
					{
						HARKLE_ERROR(Grand_Prix, main, rando_me failed);
						success = false;
						break;
					}
					else if (racerNames[tmpNameIndex])
					{
						tmpRacerName = racerNames[tmpNameIndex];
					}
					else
					{
						// Verify a name still exists in the list
						for (j = 0; j < numNames; j++)
						{
							if (racerNames[j])
							{
								break;  // Found one	
							}
						}
						// Did we find at least one left?
						if (!(racerNames[j]))
						{
							// Apparently, we're out of names
							break;
						}
					}
				} while (!tmpRacerName && true == success);
				
				// 2.1. Create struct data
				tmpMember = create_a_hThrDetails_ptr(tmpRacerName, i + 1, (void*)racer_rando_prime, NULL, 0);
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_rando_prime, NULL, 0);
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_sleepy_func, NULL, 0);
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_sleepy_func, (void*)numTrackPnts, sizeof(int));
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_func, (void*)(intptr_t)(i + 1), sizeof(int));

				if (!tmpMember)
				{
					HARKLE_ERROR(Grand_Prix, main, create_a_hThrDetails_ptr failed);
					success = false;
					break;
				}
				else
				{
					// Remove the name we chose so we don't reuse it
					// fprintf(stdout, "Before: %s\n", racerNames[tmpNameIndex]);  // DEBUGGING
					racerNames[tmpNameIndex] = NULL;
					tmpRacerName = NULL;
					// fprintf(stdout, "After:  %s\n", racerNames[tmpNameIndex]);  // DEBUGGING
				}

				// 2.1. Create a populated struct
				racerArr_ptr[i] = populate_tgpRacer_ptr(tmpMember, numTrackPnts / 2, trkHeadNode, numLaps);
				// racerArr_ptr[i] = populate_tgpRacer_ptr(tmpMember, numTrackPnts / 2, trkHeadNode, 1);
				// numTries = 0;
				// while (numTries < GRAND_PRIX_MAX_TRIES && NULL == racerArr_ptr[i])
				// {
				// 	racerArr_ptr[i] = (tgpRacer_ptr)calloc(1, sizeof(tgpRacer));
				// 	numTries++;
				// }

				if (NULL == racerArr_ptr[i])
				{
					HARKLE_ERROR(Grand_Prix, main, populate_tgpRacer_ptr failed);
					success = false;
					break;
				}
				else
				{
					// Make this thread self-aware
					racerArr_ptr[i]->F1Details->tArgvString = racerArr_ptr[i];
					racerArr_ptr[i]->F1Details->tArgSize = sizeof(tgpRacer_ptr);
					///////////////////////////////////////////
					tmpMember = NULL;  // FIX THIS LATER! //
					///////////////////////////////////////////
					// // 2.2. Populate that struct
					// racerArr_ptr[i]->F1Details = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_func, (void*)(intptr_t)(i + 1), sizeof(int));

					// if (NULL == racerArr_ptr[i]->F1Details)
					// {
					// 	HARKLE_ERROR(Grand_Prix, main, create_a_hThrDetails_ptr failed);
					// 	success = false;
					// 	break;
					// }
				}
			}
		}
	}

	// START THE RACE
	if (true == success)
	{
		// tgpRacer_ptr* racerArr_ptr = NULL;  // Array of racer struct pointers
		// tgpRacer_ptr racer_ptr = NULL;  // Index from the array of racer struct pointers
		// spawn_harklethread(racert_ptr);

		// 1. Line Up The Cars
		for (i = 0; i < numF1s; i++)
		{
			racer_ptr = racerArr_ptr[i];

			// Start the engines
			tmpInt = spawn_harklethread(racer_ptr->F1Details);

			if (tmpInt)
			{
				HARKLE_ERROR(Grand_Prix, main, spawn_harklethread failed);
				fprintf(stderr, "spawn_harklethread() returned errno:\t%s\n", strerror(tmpInt));
				success = false;
			}
			else
			{
				// fprintf(stdout, "Just spawned thread #%d\n", racer_ptr->F1Details->tNum);  // DEBUGGING
			}
		}

		// 2. Green Light
		/////////////////////////////////////// IMPLEMENT LATER ///////////////////////////////////////

		// 3. RACE!
		while (true == success)
		{
			// Give the threads a moment to execute
			sleep(SLEEPY_OFFICIALS);

			// Read what the thread has done so far
			for (i = 0; i < numF1s; i++)
			{
				racer_ptr = racerArr_ptr[i];

				// 1. Lock the mutex
				tmpInt = pthread_mutex_lock(&(racer_ptr->F1Details->pipeMutex));

				if (tmpInt)
				{
					HARKLE_ERROR(Grand_Prix, main, pthread_mutex_lock failed);
					fprintf(stderr, "pthread_mutex_lock() returned errno:\t%s\n", strerror(tmpInt));
					success = false;
					break;
				}
				else
				{
					// 2. Read the pipe
					do
					{
						// Free any previous reads
						if (pipeReads)
						{
							if (false == release_a_string(&pipeReads))
							{
								HARKLE_ERROR(Grand_Prix, main, release_a_string failed);
								success = false;
								break;
							}
						}
						// puts("ENTERING READ_A_PIPE()");  // DEBUGGING
						//////////////////////////////// BUG ////////////////////////////////
						// Second function call to pipe reads is hanging
						// I now believe that read_a_pipe() is blocking on an empty socket.
						//	With the mutex locked, it will hang forever.  Seems like I need
						//	to include some functionality inside make_a_pipe() to utilize
						//	fcntl() to set the pipe to "F_GETFL and F_SETFL plus O_NONBLOCK"
						// see: https://stackoverflow.com/questions/13811614/c-how-to-see-if-a-pipe-is-empty
						/////////////////////////////////////////////////////////////////////
						pipeReads = read_a_pipe(racer_ptr->F1Details->pipeFDs[HPIPE_READ], '\n', &errNum);

						if (!pipeReads || errNum)
						{
							// Apparently, a read from an empty pipe could return
							//	EACCES, EAGAIN, or EWOULDBLOCK
							if (errNum & (EACCES | EAGAIN | EWOULDBLOCK))
							{
								// fprintf(stdout, "Thread #%d's pipe responded '%s'", racer_ptr->F1Details->tNum, strerror(errNum));  // DEBUGGING
								continue;
							}
							else
							{
								HARKLE_ERROR(Grand_Prix, main, read_a_pipe failed);
								fprintf(stderr, "read_a_pipe() returned errno:\t%s\n", strerror(errNum));
								success = false;
								break;
							}
						}
						else
						{
							// Remove the newline
							// fprintf(stdout, "Before:\t%s\n", pipeReads);  // DEBUGGING
							if (pipeReads[strlen(pipeReads - 1)] == '\n')
							{
								pipeReads[strlen(pipeReads - 1)] = 0;
							}
							// fprintf(stdout, "After: \t%s\n", pipeReads);  // DEBUGGING

							// Convert the string to an integer
							tmpInt = atoi(pipeReads);
							// if (!tmpInt)
							// {
							// 	fprintf(stdout, "pipeReads (%s) just translated into %d.\n", pipeReads, tmpInt);  // DEBUGGING
							// }
							// fprintf(stderr, "pipeReads (%s) just translated into %d.\n", pipeReads, tmpInt);  // DEBUGGING

							// Update the struct

							// Ignore the starting positions
							// if (racer_ptr->currPos >= tmpInt && tmpInt != 1)
							// {
							// 	fprintf(stderr, "pipeReads (%s) just translated into %d.\n", pipeReads, tmpInt);  // DEBUGGING
							// 	HARKLE_ERROR(Grand_Prix, main, atoi (or the thread logic) failed);
							// 	fprintf(stderr, "Thread Racer #%d was previously at %d but is now reading at position %d.\n", \
							// 		    racer_ptr->F1Details->tNum, racer_ptr->currPos, tmpInt);  // DEBUGGING
							// 	success = false;
							// 	break;
							// }
							// else

							if (1)
							{
								// fprintf(stdout, "Thread Racer #%d was previously at %d but is now reading at position %d.\n", \
									    racer_ptr->F1Details->tNum, racer_ptr->currPos, tmpInt);  // DEBUGGING
								// fprintf(stdout, "The track size is %d\n", numTrackPnts);  // DEBUGGING
								// fprintf(stdout, "This racer is on lap %d of %d\n", racer_ptr->currLap, racer_ptr->numLaps);  // DEBUGGING
								// if (racer_ptr->trackLen == racer_ptr->currPos)
								// fprintf(stderr, "\n%d == %d && 1 == %d\n", racer_ptr->trackLen, racer_ptr->currPos, tmpInt);  // DEBUGGING
								if (racer_ptr->trackLen == racer_ptr->currPos && 1 == tmpInt)
								{
									// fprintf(stdout, "Moving up a lap from %d to %d\n", racer_ptr->currLap, racer_ptr->currLap + 1);  // DEBUGGING
									racer_ptr->currLap++;
									tmpInt++;
								}
								racer_ptr->currPos = tmpInt;
								// if (racer_ptr->currPos == racer_ptr->trackLen && racer_ptr->numLaps == racer_ptr->currLap || \
								// 	racer_ptr->currPos == 1 && racer_ptr->numLaps <= racer_ptr->currLap)
								if (racer_ptr->currPos == racer_ptr->trackLen && racer_ptr->numLaps == racer_ptr->currLap)
								// if (racer_ptr->currPos == numTrackPnts / 2 && racer_ptr->numLaps == racer_ptr->currLap)
								{
									// fprintf(stdout, "This racer is on lap %d of %d\n", racer_ptr->currLap, racer_ptr->numLaps);  // DEBUGGING
									foundWinner = true;
									racer_ptr->winner = true;
									// fprintf(stdout, "It appears that Thread #%d has won!\n", racer_ptr->F1Details->tNum);
									break;
								}
							}
						}
					} while (pipeReads);

					// 3. Unlock the mutex
					tmpInt = pthread_mutex_unlock(&(racer_ptr->F1Details->pipeMutex));

					if (tmpInt)
					{
						HARKLE_ERROR(Grand_Prix, main, pthread_mutex_unlock failed);
						fprintf(stderr, "pthread_mutex_unlock() returned errno:\t%s\n", strerror(tmpInt));
						success = false;
						break;
					}
				}
			}
			
			// Determine current lap
			highLap = highest_lap(racerArr_ptr);
			
			if (-1 == highLap)
			{
				HARKLE_ERROR(Grand_Prix, main, highest_lap failed);
				success = false;
				break;
			}

			// Update race details
			if (false == update_all_racer_pos(racerArr_ptr, trkHeadNode, highLap))
			{
				HARKLE_ERROR(Grand_Prix, main, update_all_racer_pos failed);
				success = false;
				break;
			}
			else
			{
				// puts("Updating racer positions");  // DEBUGGING
				// fprintf(stdout, "Thread #%d is at %d\n", (*racerArr_ptr)->F1Details->tNum, (*racerArr_ptr)->currPos);  // DEBUGGING
				// fprintf(stdout, "Thread #%d is at coord %d which has char %c (%d)\n", \
				        (*racerArr_ptr)->F1Details->tNum, (*racerArr_ptr)->currCoord->posNum, \
				        (*racerArr_ptr)->currCoord->graphic, (*racerArr_ptr)->currCoord->graphic);  // DEBUGGING
			}

			// UDPATE THE WINDOWS
			// Update the trackWin
			if (false == print_plot_list(trackWin->win_ptr, trkHeadNode))
			{
				HARKLE_ERROR(Grand_Prix, main, print_plot_list failed);
				success = false;
				break;
			}
			// Update the rankWin
			if (false == update_ranking_win(rankBarWin, racerArr_ptr))
			{
				HARKLE_ERROR(Grand_Prix, main, print_plot_list failed);
				success = false;
				break;
			}
			
			// PRINT THE WINDOWS
			// Print the trackWin
			if (OK != wrefresh(trackWin->win_ptr))  // Print it on the real screen
			{
				HARKLE_ERROR(Grand_Prix, main, wrefresh failed on trackWin);
				success = false;
				break;
			}
			// Print the rankWin
			if (OK != wrefresh(rankBarWin->win_ptr))
			{
				HARKLE_ERROR(Grand_Prix, main, wrefresh failed on rankBar);
				success = false;
				break;
			}
			
			// Is there a winner?
			if (foundWinner == true)
			{
				// puts("ENDED HERE");  // DEBUGGING
				break;	
			}
		}
	}
	
	// END THE RACE
	// Wait for other threads to finish
	// PLACEHOLDER
	// Print "Press any key to end race"
	if (OK != mvwaddstr(stdWin->win_ptr, 1, 1, "Press any key to end the race"))
	{
		HARKLE_ERROR(Grand_Prix.c, main, mvwaddstr failed);
	}
	getch();  // Wait for the user to press a key
	clear();  // Clear the screen
	// Print race results page
	// PLACEHOLDER
	getch();  // Wait for the user to press a key

	// CLEAN UP
	// ncurses Windows
	// 1. stdWin
	if (NULL != stdWin)
	{
		// delwin the WINDOW*
		if (OK != kill_a_window(&(stdWin->win_ptr)))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_window failed);
		}
		// free the struct
		if (false != kill_a_winDetails_ptr(&stdWin))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_winDetails_ptr failed);	
		}
	}
	// 2. trackWin
	if (NULL != trackWin)
	{
		if (OK != kill_a_window(&(trackWin->win_ptr)))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_window failed);
		}
		// free the struct
		if (false != kill_a_winDetails_ptr(&trackWin))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_winDetails_ptr failed);	
		}
	}
	// 3. rankBarWin
	if (NULL != rankBarWin)
	{
		if (OK != kill_a_window(&(rankBarWin->win_ptr)))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_window failed);
		}
		// free the struct
		if (false != kill_a_winDetails_ptr(&rankBarWin))
		{
			HARKLE_ERROR(Grand_Prix.c, main, kill_a_winDetails_ptr failed);	
		}
	}
	// 4. Restore tty modes, reset cursor location, and resets the terminal into the proper non-visual mode
	endwin();  // End curses mode
	
	// TESTING
	// racer_uint_prime_func(tmpMember, 400);
	// racer_ul_prime_func(tmpMember, 1000);
	// racer_sleepy_func(tmpMember, 100);
	// fprintf(stdout, "Thread Racer #%d was previously at %d but is now reading at position %d.\n", \
		    racer_ptr->F1Details->tNum, racer_ptr->currPos, tmpInt);  // DEBUGGING
	// fprintf(stdout, "The track size is %d\n", numTrackPnts);  // DEBUGGING
	// fprintf(stdout, "This racer is on lap %d of %d\n", racer_ptr->currLap, racer_ptr->numLaps);  // DEBUGGING

	// Allocation
	// 5. Free the racers
	if (racerArr_ptr)
	{
		for (i = 0; i < numF1s; i++)
		{
			if (racerArr_ptr[i])
			{
				if (racerArr_ptr[i]->F1Details)
				{
					// Free the hThrDetails_ptr
					if (false == free_a_hThrDetails_ptr(&(racerArr_ptr[i]->F1Details)))
					{
						HARKLE_ERROR(Grand_Prix, main, free_a_hThrDetails_ptr failed);
					}
				}
				// Free the tgpRacer_ptr
				free(racerArr_ptr[i]);
				// NULL the tgpRacer_ptr
				racerArr_ptr[i] = NULL;
			}
		}

		// Free the array
		free(racerArr_ptr);
		racerArr_ptr = NULL;
	}

	// 6. Free the track plot point array
	if (trackPntArray)
	{
		// Zeroize the memory
		if (numTrackPnts > 0)
		{
			if (trackPntArray != memset(trackPntArray, 0x0, numTrackPnts))
			{
				HARKLE_ERROR(Grand_Prix, main, memset failed);
			}
		}
		// Free the pointer
		free(trackPntArray);
		// NULL the pointer
		trackPntArray = NULL;
	}

	// 7. Free the linked list of cartCoord nodes
	if (trkHeadNode)
	{
		if (false == free_cardCoord_linked_list(&trkHeadNode))
		{
			HARKLE_ERROR(Grand_Prix, main, free_cardCoord_linked_list failed);
		}
	}

	// 8. Free this temp variable if it's still around
	if (tmpMember)
	{
		if (false == free_a_hThrDetails_ptr(&tmpMember))
		{
			HARKLE_ERROR(Grand_Prix, main, free_a_hThrDetails_ptr failed);
		}
	}

	// 9. Free this temp variable if it's still around
	if (pipeReads)
	{
		if (false == release_a_string(&pipeReads))
		{
			HARKLE_ERROR(Grand_Prix, main, release_a_string failed);
		}
	}

	// DONE
	return 0;
}


void racer_func(int racerNum)
{
	// LOCAL VARIABLES
	int count = 10;

	// COUNTDOWN
	while (count)
	{
		fprintf(stdout, "Thread #%02d - %d!\n", racerNum, count);
		count--;
	}

	// DONE
	return;
}


bool is_this_prime(unsigned long long num)
{
	// LOCAL VARIABLES
	bool retVal = true;
	unsigned long long i = 5;  // Temp var
	
	// CALCULATE
	if (num <= 1)
	{
		retVal = false;
	}
	else if (num <= 3)
	{
		retVal = true;
	}
	else if (0 == num % 2 || 0 == num % 3)
	{
		retVal = false;
	}
	else
	{
		while (i * i <= num && i <= (int)sqrt(num))
		{
			if (0 == num % i || 0 == num % (i + 2))
			{
				retVal = false;
				break;
			}
			else
			{
				i += 6;	
			}
		}
	}
	
	// DONE
	return retVal;
}
	

void racer_ull_prime_func(hThrDetails_ptr F1Details, unsigned long long trackLen)
{
	// LOCAL VARIABLES
	unsigned long long counter = 0;  // Counts down the number of primes found
	unsigned long long i = ULLONG_MAX;  // Iterating variable
	
	// INPUT VALIDATION
	if (F1Details && trackLen > 0)
	{
		counter = trackLen;
	
		// FIND PRIMES
		while (1)
		{
			// for (i = 1; i <=  ULLONG_MAX; i++)
			for (i = ULLONG_MAX; i > 0; i--)
			{
				fprintf(stdout, "Investigating %llu...\n", i);  // DEBUGGING
				if (true == is_this_prime(i))
				{
					// Decrement counter
					counter--;
					// Print the prime to the pipe
					/////////////////////////////////////////// IMPLEMENT LATER ///////////////////////////////////////////
					fprintf(stdout, "#%llu:\t%llu is prime.\n", counter, i);  // DEBUGGING
				}

				if (0 == counter)
				{
					break;
				}
			}

			if (0 == counter)
			{
				break;
			}
		}
	}
	
	// DONE
	/////////////////////// WHAT'S THE RIGHT WAY FOR A THREAD TO EXIT? ///////////////////////
	return;
}


void racer_ul_prime_func(hThrDetails_ptr F1Details, unsigned long trackLen)
{
	// LOCAL VARIABLES
	unsigned long counter = 0;  // Counts down the number of primes found
	unsigned long i = ULONG_MAX;  // Iterating variable
	unsigned long tmpVar = 5;  // Used to check for prime
	bool isThisPrime = true;  // Reset this to true
	
	puts("ENTERING racer_ul_prime_func()");  // DEBUGGING

	// INPUT VALIDATION
	if (F1Details && trackLen > 0)
	{
		counter = trackLen;
	
		// FIND PRIMES
		while (1)
		{
			// for (i = 1; i <=  ULLONG_MAX; i++)
			for (i = ULONG_MAX; i > 0; i--)
			{
				fprintf(stdout, "Investigating %lu...\n", i);  // DEBUGGING


				if (i <= 1)
				{
					isThisPrime = false;
				}
				else if (i <= 3)
				{
					isThisPrime = true;
				}
				else if (0 == i % 2 || 0 == i % 3)
				{
					isThisPrime = false;
				}
				else
				{
					while (tmpVar * tmpVar <= i && tmpVar <= (int)sqrt(i))
					{
						if (0 == i % tmpVar || 0 == i % (tmpVar + 2))
						{
							isThisPrime = false;
							break;
						}
						else
						{
							tmpVar += 6;	
						}
					}
				}

				if (true == isThisPrime)
				{
					// Decrement counter
					counter--;
					// Print the prime to the pipe
					/////////////////////////////////////////// IMPLEMENT LATER ///////////////////////////////////////////
					fprintf(stdout, "#%lu:\t%lu is prime.\n", counter, i);  // DEBUGGING
				}
				else
				{
					// Reset temp variable
					isThisPrime = true;
				}

				if (0 == counter)
				{
					break;
				}
			}

			if (0 == counter)
			{
				break;
			}
		}
	}
	
	// DONE
	/////////////////////// WHAT'S THE RIGHT WAY FOR A THREAD TO EXIT? ///////////////////////
	return;
}


void racer_uint_prime_func(hThrDetails_ptr F1Details, unsigned int trackLen)
{
	// LOCAL VARIABLES
	unsigned int counter = 0;  // Counts down the number of primes found
	unsigned int i = UINT_MAX;  // Iterating variable
	unsigned int tmpVar = 5;  // Used to check for prime
	bool isThisPrime = true;  // Reset this to true
	
	// INPUT VALIDATION
	if (F1Details && trackLen > 0)
	{
		counter = trackLen;
	
		// FIND PRIMES
		while (1)
		{
			// for (i = 1; i <=  ULLONG_MAX; i++)
			for (i = UINT_MAX; i > 0; i--)
			{
				// fprintf(stdout, "Investigating %u...\n", i);  // DEBUGGING


				if (i <= 1)
				{
					isThisPrime = false;
				}
				else if (i <= 3)
				{
					isThisPrime = true;
				}
				else if (0 == i % 2 || 0 == i % 3)
				{
					isThisPrime = false;
				}
				else
				{
					while (tmpVar * tmpVar <= i && tmpVar <= (int)sqrt(i))
					{
						if (0 == i % tmpVar || 0 == i % (tmpVar + 2))
						{
							isThisPrime = false;
							break;
						}
						else
						{
							tmpVar += 6;	
						}
					}
				}

				if (true == isThisPrime)
				{
					// Decrement counter
					counter--;
					// Print the prime to the pipe
					/////////////////////////////////////////// IMPLEMENT LATER ///////////////////////////////////////////
					// fprintf(stdout, "#%u:\t%u is prime.\n", counter, i);  // DEBUGGING
				}
				else
				{
					// Reset temp variable
					isThisPrime = true;
				}

				if (0 == counter)
				{
					break;
				}
			}

			if (0 == counter)
			{
				break;
			}
		}
	}
	
	// DONE
	/////////////////////// WHAT'S THE RIGHT WAY FOR A THREAD TO EXIT? ///////////////////////
	return;
}


void racer_sleepy_func(tgpRacer_ptr threadDets)
// void racer_sleepy_func(int trackLen)
{
	// LOCAL VARIABLES
	int counter = 0;  // Counts down the number of primes found
	int errNum = 0;  // Capture errno here during error conditions
	char localNum[SLEEPY_BUFF + 1] = { 0 };  // ULLONG_MAX as a placeholder
	*localNum = 0;  // Hide that data
	int numBytes = 0;  // Return value from snprintf()
	bool success = true;  // Set this to false if anything fails
	
	// INPUT VALIDATION
	if (!threadDets)
	{
		HARKLE_ERROR(Grand_Prix, racer_sleepy_func, NULL pointer);
		success = false;
	}
	else if (threadDets->trackLen < 0)
	{
		HARKLE_ERROR(Grand_Prix, racer_sleepy_func, Invalid track length);
		success = false;
	}

	if (true == success)
	{
		for (counter = 1; counter <= threadDets->trackLen; counter++)
		{
			// 𝄞 Why are you sleepy? ♬
			// ♩ Sleepy thread ♪
			// ♭ Thread is sleepy ♫
			// 𝄫 Sleepy thread ♫
			// sleep(SLEEPY_RACER);
			sleep(threadDets->F1Details->tNum);
			// fprintf(stdout, "Thread #%d is sleepy...  zzzZZZzzz... %d\n", threadDets->F1Details->tNum, counter);  // DEBUGGING

			// OUTPUT TO THE PIPE
			// 1. Lock the mutex
			errNum = pthread_mutex_lock(&(threadDets->F1Details->pipeMutex));

			if (errNum)
			{
				HARKLE_ERROR(Grand_Prix, racer_sleepy_func, pthread_mutex_lock failed);
				fprintf(stderr, "pthread_mutex_lock() returned errno:\t%s\n", strerror(errNum));
				success = false;
				break;
			}
			else
			{
				// 2. Write the counter
				// 2.1. Convert counter to a string
				numBytes = snprintf(localNum, SLEEPY_BUFF, "%d\n", counter);

				if (numBytes >= SLEEPY_BUFF)
				{
					HARKLE_ERROR(Grand_Prix, racer_sleepy_func, snprintf truncated the output);
					success = false;
					break;
				}
				else if (numBytes < 0)
				{
					HARKLE_ERROR(Grand_Prix, racer_sleepy_func, snprintf failed);
					success = false;
					break;
				}
				else
				{
					// 2.2. Write that string to the pipe
					errNum = write_a_pipe(threadDets->F1Details->pipeFDs[HPIPE_WRITE], (void*)localNum, numBytes);

					if (errNum)
					{
						HARKLE_ERROR(Grand_Prix, racer_sleepy_func, write_a_pipe failed);
						fprintf(stderr, "write_a_pipe() returned errno:\t%s\n", strerror(errNum));
						success = false;
						break;
					}
				}

				// 3. Unlock the mutex
				errNum = pthread_mutex_unlock(&(threadDets->F1Details->pipeMutex));

				if (errNum)
				{
					HARKLE_ERROR(Grand_Prix, racer_sleepy_func, pthread_mutex_unlock failed);
					fprintf(stderr, "pthread_mutex_lock() returned errno:\t%s\n", strerror(errNum));
					success = false;
					break;
				}
			}
		}
	}

	return;
}


void racer_rando_prime(tgpRacer_ptr threadDets)
{
	// LOCAL VARIABLES
	int counter = 0;  // Counts the number of primes found
	int currentLap = 1;  // Counts the number of laps this thread has run
	int subCounter = 0;  // Require more calculations than before
	int fastMult = 0;  // Multiple to increase the number of calculations, minimum 1
	int errNum = 0;  // Capture errno here during error conditions
	char localNum[SLEEPY_BUFF + 1] = { 0 };  // ULLONG_MAX as a placeholder
	int numBytes = 0;  // Return value from snprintf()
	bool success = true;  // Set this to false if anything fails
	bool isThisPrime = true;  // Reset this to true
	////////////////////// DATA TYPE DEPENDENT VARIABLES /////////////////////
	// uint randoNum = 0;  // Holds a psuedo-random number
	// unsigned long randoNum = 0;  // Holds a psuedo-random number
	volatile unsigned long long randoNum = 0;  // Holds a psuedo-random number
	// unsigned int tmpVar = 5;  // Used to check for prime
	// unsigned long tmpVar = 5;  // Used to check for prime
	unsigned long long tmpVar = 5;  // Used to check for prime
	//////////////////////////////////////////////////////////////////////////
	
	// INPUT VALIDATION
	if (!threadDets)
	{
		HARKLE_ERROR(Grand_Prix, racer_rando_prime, NULL pointer);
		success = false;
	}
	else if (threadDets->trackLen < 0)
	{
		HARKLE_ERROR(Grand_Prix, racer_rando_prime, Invalid track length);
		success = false;
	}
	else
	{
		if (FAST_RACER >= 1)
		{
			fastMult = FAST_RACER;
		}
		else
		{
			fastMult = 1;
		}
	}

	if (true == success)
	{
		while (counter != threadDets->trackLen || currentLap != threadDets->numLaps)
		{
			// 𝄞 Why are you sleepy? ♬
			// ♩ Sleepy thread ♪
			// ♭ Thread is sleepy ♫
			// 𝄫 Sleepy thread ♫
			sleep(SLEEPY_RACER);
			// sleep(threadDets->F1Details->tNum);
			// fprintf(stdout, "Thread #%d is sleepy...  zzzZZZzzz... %d\n", threadDets->F1Details->tNum, counter);  // DEBUGGING

			// FIND A RANDOM PRIME
			while (1)
			{
				isThisPrime = true;  // Reset temp var
				// randoNum = rando_a_uint(1, UINT_MAX);
				// randoNum = rando_a_ulong(1, ULONG_MAX);
				randoNum = rando_a_ullong(1, ULLONG_MAX);
				// fprintf(stdout, "Thread #%d got %u.\n", threadDets->F1Details->tNum, randoNum);  // DEBUGGING
				// fprintf(stdout, "Thread #%d got %lu.\n", threadDets->F1Details->tNum, randoNum);  // DEBUGGING
				// fprintf(stdout, "Thread #%d got %llu.\n", threadDets->F1Details->tNum, randoNum);  // DEBUGGING

				if (randoNum <= 1)
				{
					isThisPrime = false;
				}
				else if (randoNum <= 3)
				{
					isThisPrime = true;
				}
				else if (0 == randoNum % 2 || 0 == randoNum % 3)
				{
					isThisPrime = false;
				}
				else
				{
					while (tmpVar * tmpVar <= randoNum && tmpVar <= (unsigned long long)sqrt(randoNum))
					// while (tmpVar * tmpVar <= randoNum && tmpVar <= (unsigned int)sqrt(randoNum))
					{
						if (0 == randoNum % tmpVar || 0 == randoNum % (tmpVar + 2))
						{
							isThisPrime = false;
							break;
						}
						else
						{
							tmpVar += 6;	
						}
					}
				}

				if (true == isThisPrime)
				{
					subCounter++;
					if (0 == subCounter % fastMult)
					{
						// counter++;
						subCounter = 0;  // Reset temp var

						if (counter == threadDets->trackLen && currentLap < threadDets->numLaps)
						{
							counter = 1;
							currentLap++;
							// threadDets->currLap++;
							// if (threadDets->F1Details->tNum == 1)
							// {
							// 	fprintf(stderr, "Thread #%d is looping at lap %d, position %d.\n", threadDets->F1Details->tNum, currentLap, counter);  // DEBUGGING
							// }
						}
						else
						{
							counter++;
							// if (threadDets->F1Details->tNum == 1)
							// {
							// 	fprintf(stderr, "Thread #%d is at lap %d, position %d.\n", threadDets->F1Details->tNum, currentLap, counter);  // DEBUGGING
							// }
						}
						break;
					}
					// fprintf(stdout, "Thread #%d found prime #%d:\t%u.\n", threadDets->F1Details->tNum, counter, randoNum);  // DEBUGGING
					// fprintf(stdout, "Thread #%d found prime #%d:\t%lu.\n", threadDets->F1Details->tNum, counter, randoNum);  // DEBUGGING
					// fprintf(stdout, "Thread #%d found prime #%d:\t%llu.\n", threadDets->F1Details->tNum, counter, randoNum);  // DEBUGGING
				}
			}

			// OUTPUT TO THE PIPE
			// 1. Lock the mutex
			errNum = pthread_mutex_lock(&(threadDets->F1Details->pipeMutex));

			if (errNum)
			{
				HARKLE_ERROR(Grand_Prix, racer_rando_prime, pthread_mutex_lock failed);
				fprintf(stderr, "pthread_mutex_lock() returned errno:\t%s\n", strerror(errNum));
				success = false;
				break;
			}
			else
			{
				// 2. Write the counter
				// 2.1. Convert counter to a string
				numBytes = snprintf(localNum, SLEEPY_BUFF, "%d\n", counter);

				if (numBytes >= SLEEPY_BUFF)
				{
					HARKLE_ERROR(Grand_Prix, racer_rando_prime, snprintf truncated the output);
					success = false;
					break;
				}
				else if (numBytes < 0)
				{
					HARKLE_ERROR(Grand_Prix, racer_rando_prime, snprintf failed);
					success = false;
					break;
				}
				else
				{
					// 2.2. Write that string to the pipe
					errNum = write_a_pipe(threadDets->F1Details->pipeFDs[HPIPE_WRITE], (void*)localNum, numBytes);

					if (errNum)
					{
						HARKLE_ERROR(Grand_Prix, racer_rando_prime, write_a_pipe failed);
						fprintf(stderr, "write_a_pipe() returned errno:\t%s\n", strerror(errNum));
						success = false;
						break;
					}
				}

				// 3. Unlock the mutex
				errNum = pthread_mutex_unlock(&(threadDets->F1Details->pipeMutex));

				if (errNum)
				{
					HARKLE_ERROR(Grand_Prix, racer_rando_prime, pthread_mutex_unlock failed);
					fprintf(stderr, "pthread_mutex_lock() returned errno:\t%s\n", strerror(errNum));
					success = false;
					break;
				}
			}
		}
	}

	// if (threadDets->F1Details->tNum == 1)
	// {
	// 	fprintf(stderr, "Thread #%d is exiting at lap %d, position %d.\n", threadDets->F1Details->tNum, currentLap, counter);  // DEBUGGING
	// }

	return;
}


/*
attron/attroff bit-mask attributes

A_NORMAL 		Normal display (no highlight)
A_STANDOUT 		Best highlighting mode of the terminal.
A_UNDERLINE 	Underlining
A_REVERSE 		Reverse video
A_BLINK 		Blinking
A_DIM 			Half bright
A_BOLD 			Extra bright or bold
A_PROTECT 		Protected mode
A_INVIS 		Invisible or blank mode
A_ALTCHARSET 	Alternate character set
A_CHARTEXT 		Bit−mask to extract a character
COLOR_PAIR(n) 	Color−pair number n
 */
