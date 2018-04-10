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

/*
typdef struct hcCartesianCoordinate
{
	int absX;								// X coordinate starting at window's top left
	int absY;								// Y coordinate starting at window's top left
	char graphic;							// Character to print at this coordinate
	unsigned long hcFlags;					// Implementation-defined coordinate details
	struct hcCartesianCoordinate* nextPnt;  // Next node in the linked list
} hcCartCoord, *hcCartCoord_ptr;
 */

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

////////////////////// CARTESIAN COORDINATE STRUCT START /////////////////////


/*
	PURPOSE - Allocate heap memory for one hcCartesianCoordinate struct
	INPUT - None
	OUTPUT
		On success, pointer to a hcCartesianCoordinate struct on the heap
		On failure, NULL
	NOTES
		It is the caller's responsibility to free the memory allocated by this function call
 */
hcCartCoord_ptr allocate_cartCoord_struct(void)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Initialize a newly heap-allocated hcCartesianCoordinate struct
	INPUT
		xVal - Absolute, from the ncurse window's top left, x coordinate of this point
		yVal - Absolute, from the ncurse window's top left, y coordinate of this point
		pntChar - The character to print at coordinate (xVal, yVal)
		initStatus - Initial value of the implementation-defined flags available for this point
	OUTPUT
		On success, pointer to a hcCartesianCoordinate struct on the heap that has been
			populated with the build_new_cartCoord_struct() arguments
		On failure, NULL
	NOTES
		This function calls allocate_cartCoord_struct() to allocate memory for the struct
		It is the caller's responsibility to free the memory allocated by this function call
 */
hcCartCoord_ptr build_new_cartCoord_struct(int xVal, int yVal, char pntChar, unsigned long initStatus)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	
	// DONE
	return retVal;
}


/*
	PURPOSE - This function will add a new cartCoord node (hcCartesianCoordinate struct) to
		a linked list of cartCoord nodes, if one already exists.
	INPUT
		headPnt - hcCartesianCoordinate struct pointer to an existing head node, if any.  If
			headPnt is NULL, newPnt will become the head node of a linked list
		newPnt - hcCartesianCoordinate struct pointer to a new node to add to the existing
			link list, if any
		pntPos - Position to add newPnt:
			0 - Add newPnt to the end
			X - Insert newPnt in front of node #X
	OUTPUT
		On success, pointer to the head node of a linked list of hcCartesianCoordinate structs.
			If headPnt is NULL, newPnt will be returned.  Otherwise, headPnt will be returned.
		On failure, NULL
	NOTES
		If pntPos exceeds the length of the linked list, this function will just add newPnt
			to the end.
		Take care not to lose the pointer to your head node in case this function experiences
			an error.  PRO TIP: Use a temp variable to store this function's return value.
 */
hcCartCoord_ptr add_cartCoord_node(hcCartCoord_ptr headPnt, hcCartCoord_ptr newPnt, int pntPos)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Count the number of nodes in the linked list of hcCartesianCoordinate structs
		starting at headPnt
	INPUT
		headPnt - headPnt - hcCartesianCoordinate struct pointer to an existing head node,
			if any
	OUTPUT
		On success, number of nodes in the linked list.  If headPnt is NULL, 0.
		On error, -1
 */
int get_num_cartCoord_nodes(hcCartCoord_ptr headPnt)
{
	// LOCAL VARIABLES
	int retVal = -1;  // Default return value if any input validation fails
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Free the heap-allocated memory associated with a hcCartesianCoordinate struct
	INPUT
		oldStruct_ptr - A pointer to a heap-allocated hcCartesianCoordinate struct pointer
	OUTPUT
		On success, true
		On failure, false
	NOTES
		This function will attempt to zeroize and free the memory at *oldStruct_ptr.  It
			will also set the original pointer to NULL.  Call this function as
			free_cartCoord_struct(&myCartCoord_ptr);
 */
bool free_cartCoord_struct(hcCartCoord_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails
	
	// DONE
	return retVal;
}


/*
	PURPOSE - Free all of the hcCartesianCoordinate struct nodes contained in the
		linked list that starts at the given head node
	INPUT
		oldHeadNode_ptr - A pointer to the head node pointer of a hcCartesianCoordinate
			struct linked list
	OUTPUT
		On success, true
		On failure, false
	NOTES
		This function will attempt to zeroize, free, and NULL all of the memory of
			all the nodes in the linked list starting at *oldHeadNode_ptr.  Call 
			this function as free_cardCoord_linked_list(&myHeadNode_ptr);
		This function is recursive.
		This function is also recursive.
 */
bool free_cardCoord_linked_list(hcCartCoord_ptr* oldHeadNode_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails
	
	// DONE
	return retVal;
}


////////////////////// CARTESIAN COORDINATE STRUCT STOP //////////////////////

///////////////////////// NCURSES WINDOW STRUCT START ////////////////////////


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


///////////////////////// NCURSES WINDOW STRUCT STOP /////////////////////////

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
