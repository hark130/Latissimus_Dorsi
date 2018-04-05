#include "Harklecurse.h"		// winDetails, winDetails_ptr
#include "Harklerror.h"
#include "Memoroad.h"
#include <ncurses.h>			// WINDOW
#include <stdbool.h>			// bool, true, false
#include <stdlib.h>				// calloc()


#ifndef HARKLECURSE_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HARKLECURSE_MAX_TRIES 3
#endif  // HARKLECURSE_MAX_TRIES

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


winDetails_ptr build_a_winDetails_ptr(void)
{
	// LOCAL VARIABLES
	winDetails_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE MEMORY
	while (numTries < HARKLECURSE_MAX_TRIES && NULL == retVal)
	{
		retVal = (winDetails_ptr)calloc(1, sizeof(winDetails));
		numTries++;
	}

	// DONE
	return retVal;
}


winDetails_ptr populate_a_winDetails_ptr(int numRows, int numCols, int startY, int startX)
{
	// LOCAL VARIABLES
	winDetails_ptr retVal = NULL;
	bool success = true;  // Set this to false if anything fails
	int stdRows = 0;  // Total number of rows available
	int stdCols = 0;  // Total number of columns available

	// DETERMINE MAX COLS AND ROWS
	getmaxyx(stdscr, stdRows, stdCols);  // Determine the maximum dimensions
	if (ERR == stdRows || ERR == stdCols)
	{
		HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, getmaxyx failed);
		success = false;
	}

	// INPUT VALIDATION
	if (numRows < 0 || numRows > stdRows || ERR == numRows)
	{
		HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, Invalid numRows);
		success = false;
	}
	// else if (numCols < 0 || ERR == numCols)
	else if (numCols < 0 || numCols > stdCols || ERR == numCols)
	{
		HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, Invalid numCols);
		success = false;
	}
	else if (startY < 0 || ERR == startY)
	// else if (startY < 0 || startY > numRows || ERR == startY)
	{
		HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, Invalid startY);
		success = false;
	}
	else if (startX < 0 || ERR == startX)
	// else if (startX < 0 || startX > numCols || ERR == startX)
	{
		// printf("numCols was:   \t%d\n", numCols);  // DEBUGGING
		// printf("bad startX was:\t%d\n", startX);  // DEBUGGING
		HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, Invalid startX);
		success = false;
	}

	// ALLOCATE STRUCT
	if (true == success)
	{
		retVal = build_a_winDetails_ptr();

		if (!retVal)
		{
			HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, build_a_winDetails_ptr failed);
			success = false;
		}
	}

	// CREATE WINDOW
	if (true == success)
	{
		// printf("Rows:\t%d\nCols:\t%d\nY:   \t%d\nX:   \t%d\n", numRows, numCols, startY, startX);  // DEBUGGING
		retVal->win_ptr = make_a_window(numRows, numCols, startY, startX);

		if (retVal->win_ptr)
		{
			retVal->nRows = numRows;
			retVal->nCols = numCols;
			retVal->upperR = startY;
			retVal->leftC = startX;
		}
		else
		{
			HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, make_a_window failed);
			success = false;
		}
	}

	// CLEAN UP
	if (false == success && retVal)
	{
		if (false == kill_a_winDetails_ptr(&retVal))
		{
			HARKLE_ERROR(Harklecurse, populate_a_winDetails_ptr, kill_a_winDetails_ptr failed);
		}
	}

	// DONE
	return retVal;
}


bool kill_a_winDetails_ptr(winDetails_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	winDetails_ptr temp_ptr = NULL;

	// INPUT VALIDATION
	if (!oldStruct_ptr || NULL == *oldStruct_ptr)
	{
		HARKLE_ERROR(Harklecurse, kill_a_winDetails_ptr, NULL pointer);
		retVal = false;
	}

	// ZEROIZE STRUCT
	if (true == retVal)
	{
		temp_ptr = *oldStruct_ptr;

		// 1. WINDOW* win_ptr;		// ncurses WINDOWS pointer
		temp_ptr->win_ptr = NULL;

		// 2. int nRows;			// Number of rows
		temp_ptr->nRows = 0;

		// 3. int nCols;			// Number of columns
		temp_ptr->nCols = 0;

		// 4. int upperR;			// Upper most row
		temp_ptr->upperR = 0;

		// 5. int leftC;			// Left most column
		temp_ptr->leftC = 0;
	}

	// FREE STRUCT
	if (true == retVal)
	{
		free(temp_ptr);
		temp_ptr = NULL;
	}

	// NULL STRUCT
	if (true == retVal)
	{
		*oldStruct_ptr = NULL;
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// NCURSES FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////


WINDOW* make_a_window(int numRows, int numCols, int startY, int startX)
{
	// LOCAL VARIABLES
	WINDOW* retVal = NULL;
	bool success = true;  // Set this to false if anything fails
	int stdRows = 0;  // Total number of rows available
	int stdCols = 0;  // Total number of columns available

	// DETERMINE MAX COLS AND ROWS
	getmaxyx(stdscr, stdRows, stdCols);  // Determine the maximum dimensions
	if (ERR == stdRows || ERR == stdCols)
	{
		HARKLE_ERROR(Harklecurse, make_a_window, getmaxyx failed);
		success = false;
	}

	// INPUT VALIDATION
	if (numRows < 0 || numRows > stdRows || ERR == numRows)
	{
		HARKLE_ERROR(Harklecurse, make_a_window, Invalid numRows);
		success = false;
	}
	else if (numCols < 0 || numCols > stdCols || ERR == numCols)
	{
		HARKLE_ERROR(Harklecurse, make_a_window, Invalid numCols);
		success = false;
	}
	else if (startY < 0 || ERR == startY)
	// else if (startY < 0 || startY > numRows || ERR == startY)
	{
		HARKLE_ERROR(Harklecurse, make_a_window, Invalid startY);
		success = false;
	}
	else if (startX < 0 || ERR == startX)
	// else if (startX < 0 || startX > numCols || ERR == startX)
	{
		HARKLE_ERROR(Harklecurse, make_a_window, Invalid startX);
		success = false;
	}

	// CREATE WINDOW
	if (true == success)
	{
		// retVal = newwin(numRows, numCols, startY, startX);

		// OMINOUS MAN PAGE ENTRY:
		//	When using this routine, it is necessary to call 
		//	touchwin or touchline on orig before calling wrefresh 
		//	on the subwindow. 
		retVal = subwin(stdscr, numRows, numCols, startY, startX);

		if (!retVal)
		{			
			// HARKLE_ERROR(Harklecurse, make_a_window, newwin failed);
			HARKLE_ERROR(Harklecurse, make_a_window, subwin failed);
			success = false;
		}
	}

	// DONE
	return retVal;
}


int kill_a_window(WINDOW** oldWin_ptr)
{
	// LOCAL VARIABLES
	bool success = true;  // Make this false if anything fails
	int retVal = OK;  // Function's return value
	WINDOW* temp_ptr = NULL;
	int nRetVal = OK;  // ncurse function return values
	
	// INPUT VALIDATION
	if (NULL == oldWin_ptr || NULL == *oldWin_ptr)
	{
		HARKLE_ERROR(Harklecurse, kill_a_window, NULL pointer);
		success = false;
		retVal = ERR;
	}
	else
	{
		temp_ptr = *oldWin_ptr;
	}
	
	// ERASE WINDOW
	// 1. Set the window border invisible
	if (true == success)
	{
		nRetVal = wborder(temp_ptr, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
		
		if (OK != nRetVal)
		{
			HARKLE_ERROR(Harklecurse, kill_a_window, wborder failed);
			success = false;
			retVal = nRetVal;
		}
	}
	
	// 2. Make the window border invisible
	if (true == success)
	{
		nRetVal = wrefresh(temp_ptr);
		
		if (ERR == nRetVal)
		{
			HARKLE_ERROR(Harklecurse, kill_a_window, wrefresh failed);
			success = false;
			retVal = nRetVal;
		}
	}
	
	// 3. Delete the window
	// Attempt to delete this window even if something failed previously
	if (true == success || temp_ptr)
	{
		// delwin() will not succeed if if the window is the parent of another window
		nRetVal = delwin(temp_ptr);
		
		if (ERR == nRetVal)
		{
			HARKLE_ERROR(Harklecurse, kill_a_window, delwin failed);
			success = false;
			retVal = nRetVal;
		}
		else
		{
			// 4. NULL the original variable
			*oldWin_ptr = NULL;
		}
	}
	
	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// NCURSES FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
