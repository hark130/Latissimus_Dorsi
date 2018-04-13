/*
  The intent of this project is to 'race' threads while displaying their near-real-time status on a ncurses-drawn race track.
  
// DESIGN
1. Visual Design
	1. Racetrack
		1. Size
			* Dynamic based on window size
		2. Shape
			* Monaco Grand Prix
		3. "Car" indicators
			* Color circles
			* Color numbers
			* Current winner bolded
		4. Color
			* White
			* Bold
			* Dynamic?
		4. Appearance
			* Surround in a box()
	2. Rank Bar
		1. Size
			* One entire edge
		2. Location
			* Right
			* Left
			* Bottom
		3. Data displayed
			* Green plus/red minus indicating change in status?
			* Thread name
			* Thread ranking
			* Thread lap
		4. Appearance
			* Surround in a box()
	3. Final Results
		1. Data displayed
			* Thread name
			* Thread lap at end
			* Thread total time
2. Program Design
	1. Main
		1. Support for CLI args
			* -n Number of threads (10)
			* -d Length of race (161.734 mi)
			* -u Update interval (1 sec)
			* -l Number of laps (78)
		2. Determine winner
			* First to exit()?
			* Photo finish (time)?
			
// TASKS
[ ] Thread function
	[ ] Calculate something
	[ ] Check mutex and wait
	[ ] Update pipe
	[ ] Loop
[ ] Main thread function
	[ ] Sleep for an update interval
	[ ] Lock all the mutexes
	[ ] Read all the pipes
	[ ] Update the race
	[ ] Unlock all the pipes
	[ ] Loop
[X] Harklethread.h (Thread Library)
[X] Harklepipe.h (Pipe Library)
[ ] Mini map
	[ ] Draw it
	[ ] Dynamically size it
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
- Current flow design
	1. main() determines track window size (Local logic)
	2. main() determines maximum axis based on window size (Local logic)
	3. main() determines center (Harklemath helper function?  Local logic?)
	4. main() calls HMath plot_ellipse_points()
	5. main() calls build_geometric_list(double* relEllipseCoords, centX, centY)
		6. build_geometic_list(head*, x, y), for each (x, y) in ellipseCoords calls:
			7. resolve_x_coord(relX, cntX)
			8. resolve_y_coord(relY, cntY)
			9. add_cartCoord_node(absX, absY, '*', 0x0) which calls:
				10. build_cardCood_node() which calls...
					11. ...allocate_cardCoord_node() to allocate memory...
					12. ...and then assigns info to the struct
				13. add_cartCoord_node() calls insert_cartCoord_node(0) (0 for end, X for "in front of node X")
						14. insert_cardCoord_node() calls find_end_cardCoord_node() and adds the node
- Design Harklecurse struct to keep track of plot points
	- int xRelCoord
	- int yRelCoord
	- int xCntCoord
	- int yCntCoord
	- char graphic
	- unsigned long pntStatus
- Write Harklecurse function to convert double tuple list to plottable Harklecurse struct
 */

#include "Harklecurse.h"		// kill_a_window()
#include "Harklemath.h"			// determine_center(), NUM_PRIMES_ULLONG
#include "Harklerror.h"			// HARKLE_ERROR()
#include "Harklethread.h"
#include <limits.h>				// ULLONG_MAX
#include <math.h>				// sqrt()
#include <ncurses.h>			// initscr(), refresh(), endwin()
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
#define RANK_BAR_WIDTH 25

#ifndef GRAND_PRIX_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define GRAND_PRIX_MAX_TRIES 3
#endif // GRAND_PRIX_MAX_TRIES

#define SLEEPY_RACER 1  // Number of seconds for racer_sleepy_func() to sleep

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


int main(int argc, char** argv)
{
	// LOCAL VARIABLES
	//////////////////////////// Program Defaults ////////////////////////////
	int numF1s = 10;  // Number of 'racing' threads to be spawned
	int raceLen = 162;  // Length of the race in miles, rounded up
	int upInterval = 1;  // Frequency of main thread updates
	int numLaps = 78;  // Number of the laps the 'racing' threads must take
	//////////////////////////////////////////////////////////////////////////
	int retVal = 0;  // Function's return value, also holds ncurses return values
	bool winner = false;  // Update to true if any thread wins
	bool success = true;  // Make this false if anything fails
	winDetails_ptr stdWin = NULL;  // hCurseWinDetails struct pointer for the stdscr window
	winDetails_ptr trackWin = NULL;  // hCurseWinDetails struct pointer for the track window
	winDetails_ptr rankBarWin = NULL;  // hCurseWinDetails struct pointer for the rank bar window
	int numCols = 0;  // Number of columns available
	int numRows = 0;  // Number of rows available
	int i = 0;  // Iterating variable
	int numTries = 0;  // Counter for memory allocation function calls
	int tmpInt = 0;  // Holds various return values
	// Race Cars
	tgpRacer_ptr* racerArr_ptr = NULL;  // Array of racer struct pointers
	tgpRacer_ptr racer_ptr = NULL;  // Index from the array of racer struct pointers
	hThrDetails_ptr tmpMember = NULL;  // Temp variable to hold the F1Details during creation
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
				// 2.1. Create struct data
				tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_sleepy_func, NULL, 0);
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_sleepy_func, (void*)numTrackPnts, sizeof(int));
				// tmpMember = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_func, (void*)(intptr_t)(i + 1), sizeof(int));

				if (!tmpMember)
				{
					HARKLE_ERROR(Grand_Prix, main, create_a_hThrDetails_ptr failed);
					success = false;
					break;
				}

				// 2.1. Create a populated struct
				racerArr_ptr[i] = populate_tgpRacer_ptr(tmpMember, numTrackPnts);
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

		// 3. Checkered Flag
		while (1)
		{
			// Update race details
			winner = true;  // PLACEHOLDER
			
			// Print updates
			refresh();  // Print it on the real screen
			
			// Is there a winner?
			if (winner == true)
			{
				break;	
			}
		}
	}
	
	// END THE RACE
	// Wait for other threads to finish
	// PLACEHOLDER
	// Print "Press any key to end race"
	// PLACEHOLDER
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
	
	// INPUT VALIDATION
	if (!threadDets)
	{
		HARKLE_ERROR(Grand_Prix, racer_sleepy_func, NULL pointer);
	}
	else if (threadDets->trackLen < 0)
	{
		HARKLE_ERROR(Grand_Prix, racer_sleepy_func, Invalid track length);
	}
	else
	{
		for (counter = 1; counter <= threadDets->trackLen; counter++)
		{
			// 𝄞 Why are you sleepy? ♬
			// ♩ Sleepy thread ♪
			// ♭ Thread is sleepy ♫
			// 𝄫 Sleepy thread ♫
			sleep(SLEEPY_RACER);
			// fprintf(stdout, "Thread #%d is sleepy...  zzzZZZzzz... %d\n", threadDets->F1Details->tNum, counter);  // DEBUGGING

			// Print the counter to the pipe
			/////////////////////////////////////////// IMPLEMENT LATER ///////////////////////////////////////////
		}
	}

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
