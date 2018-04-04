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
	[ ] Draw it
	[ ] Dynamically size it
	[ ] Populate it
		[ ] Thread names
		[ ] Thread colors
		[ ] Thread details
			[ ] Thread placing
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

#include <ncurses.h>			// initscr(), refresh(), endwin()
#include <stdbool.h>			// bool, true, false


int main(int argc, char** argv)
{
	// LOCAL VARIABLES
	bool winner = false;  // Update to true if any thread wins
	
	// INPUT VALIDATION
	
	// BUILD RACE CARS
	
	// PRINT THE TRACK
	initscr();  // Start curses mode
	raw();  // Line buffering disabled
	noecho();  // Disable echo
	
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
	endwin();  // End curses mode
	
	// DONE
	return 0;
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
