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
[ ] Harklethread.h (Thread Library)
[ ] Harklepipe.h (Pipe Library)
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
		
 */

#include "Harklecurse.h"		// kill_a_window()
#include "Harklerror.h"			// HARKLE_ERROR()
#include "Harklethread.h"
#include <ncurses.h>			// initscr(), refresh(), endwin()
#include <stdbool.h>			// bool, true, false
#include <stdint.h>				// intptr_t
#include <stdio.h>				// printf
#include <stdlib.h>				// calloc()

#define OUTER_BORDER_WIDTH_H 4
#define OUTER_BORDER_WIDTH_V 2
#define INNER_BORDER_WIDTH_H 4
#define INNER_BORDER_WIDTH_V 2
#define RANK_BAR_WIDTH 25

#ifndef GRAND_PRIX_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define GRAND_PRIX_MAX_TRIES 3
#endif // GRAND_PRIX_MAX_TRIES

typedef struct threadGrandPrixRace
{
	hThrDetails_ptr F1Details;		// Detail regarding a 'racing' thread
} tgpRacer, *tgpRacer_ptr;


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
	tgpRacer_ptr* racerArr_ptr = NULL;  // Array of racer struct pointers
	tgpRacer_ptr racer_ptr = NULL;  // Index from the array of racert struct pointers
	
	// INPUT VALIDATION
	if (numF1s < 1)
	{
		HARKLE_ERROR(Grand_Prix, main, Invalid parameter);
		success = false;
	}

	// BUILD RACE CARS
	if (true == success)
	{
		// 1. Allocate an array for the racers
		while (numTries < GRAND_PRIX_MAX_TRIES && NULL == racerArr_ptr)
		{
			racerArr_ptr = (tgpRacer_ptr*)calloc(numF1s, sizeof(tgpRacer_ptr));
			numTries++;
		}

		if (NULL == racerArr_ptr)
		{
			HARKLE_ERROR(Grand_Prix, main, calloc failed);
			success = false;
		}
		else
		{
			// 2. Create the racers
			for (i = 0; i < numF1s; i++)
			{
				// 2.1. Allocate a struct
				numTries = 0;
				while (numTries < GRAND_PRIX_MAX_TRIES && NULL == racerArr_ptr[i])
				{
					racerArr_ptr[i] = (tgpRacer_ptr)calloc(1, sizeof(tgpRacer));
					numTries++;
				}

				if (NULL == racerArr_ptr[i])
				{
					HARKLE_ERROR(Grand_Prix, main, calloc failed);
					success = false;
					break;
				}
				else
				{
					// 2.2. Populate that struct
					racerArr_ptr[i]->F1Details = create_a_hThrDetails_ptr(NULL, i + 1, (void*)racer_func, (void*)(intptr_t)(i + 1), sizeof(int));

					if (NULL == racerArr_ptr[i]->F1Details)
					{
						HARKLE_ERROR(Grand_Prix, main, create_a_hThrDetails_ptr failed);
						success = false;
						break;
					}
				}
			}
		}
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

	// START THE RACE
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
	// 3. Restore tty modes, reset cursor location, and resets the terminal into the proper non-visual mode
	endwin();  // End curses mode

	// CLEAN UP
	// Free the racers
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
