#ifndef __HARKLECURSE__
#define __HARKLECURSE__

#include <ncurses.h>			// WINDOW

typedef struct hCurseWinDetails
{
	WINDOW* win_ptr;	// ncurses WINDOWS pointer
	int nRows;			// Number of rows
	int nCols;			// Number of columns
	int upperR;			// Upper most row
	int leftC;			// Left most column
} winDetails, *winDetails_ptr;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Allocate heap memory for a hCurseWinDetails struct pointer
	INPUT - None
	OUTPUT
		On success, a pointer to a hCurseWinDetails struct
		On failure, NULL
	NOTES
		It is the caller's responsibility to free the memory allocated here
 */
winDetails_ptr build_a_winDetails_ptr(void);


/*
	PURPOSE - Populate a newly heap-allocated hCurseWinDetails struct pointer
		with details about the ncurses window created
	INPUT
		numRows - Number of rows
		numCols - Number of columns
		startY - Upper left hand corner is in this row
		startX - Upper left hand corner is in this column
	OUTPUT
		On success, heap-allocated hCurseWinDetails struct pointer
		On failure, NULL
	NOTES
		This function calls build_a_winDetails_ptr()
		It is the caller's responsibility to free the memory allocated here

 */
winDetails_ptr populate_a_winDetails_ptr(int numRows, int numCols, int startY, int startX);


/*
	PURPOSE - Zeroize, free, and NULL an old hCurseWinDetails struct pointer
	INPUT
		oldStruct_ptr - A pointer to a hCurseWinDetails struct pointer
	OUTPUT
		On success, true
		On failure, false
	NOTES
		It is the caller's responsibility to 'tear down' the ncurses window
			referenced by this struct pointer prior to free()ing the pointer.
			This function will not attempt to call delwin().

 */
bool kill_a_winDetails_ptr(winDetails_ptr* oldStruct_ptr);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// NCURSES FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Extricate ncurses windows creation
	INPUT
		height - Number of rows
		width - Number of columns
		startY - Upper left hand corner is in this row
		startX - Upper left hand corner is in this column
	OUTPUT
		On success, pointer to a WINDOW object
		On failure, NULL
	NOTES
		This function will attempt to verify height and width do not exceed stdscr
 */
WINDOW* make_a_window(int numRows, int numCols, int startY, int startX);


/*
	PURPOSE - Erase an ncurses window
	INPUT
		oldWin_ptr - Pointer to a WINDOW*
	OUTPUT
		On success, ncurses.h macro OK
		On failure, error value from ncurses (!OK)
	NOTES:
		This function will zeroize the original pointer variable... for safety 
 */
int kill_a_window(WINDOW** oldWin_ptr);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// NCURSES FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __HARKLECURSE__
