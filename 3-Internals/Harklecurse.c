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


hcCartCoord_ptr allocate_cartCoord_struct(void)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	int numTries = 0;  // Counts the number of allocation attempts
	
	// ALLOCATION
	while (numTries < HARKLECURSE_MAX_TRIES && NULL == retVal)
	{
		retVal = (hcCartCoord_ptr)calloc(1, sizeof(hcCartCoord));
		numTries++;
	}
	
	if (NULL == retVal)
	{
		HARKLE_ERROR(Harklecurse, allocate_cartCoord_struct, calloc failed);
	}
	
	// DONE
	return retVal;
}


hcCartCoord_ptr build_new_cartCoord_struct(int xVal, int yVal, char pntChar, unsigned long initStatus)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;
	bool success = true;  // If anything fails, set this to false
	int numTries = 0;  // Number of allocation attempts
	
	// INPUT VALIDATION
	if (xVal < 0)
	{
		// printf("\nInvalid xVal == %d\n", xVal);  // DEBUGGING
		HARKLE_ERROR(Harklecurse, build_new_cartCoord_struct, Invalid xVal);
		success = false;
	}
	else if (yVal < 0)
	{
		HARKLE_ERROR(Harklecurse, build_new_cartCoord_struct, Invalid yVal);
		success = false;
	}
	else if (pntChar < 0 || pntChar > 255)
	{
		HARKLE_ERROR(Harklecurse, build_new_cartCoord_struct, Invalid pntChar);
		success = false;
	}
	
	// STRUCT ALLOCATION
	if (true == success)
	{
		retVal = allocate_cartCoord_struct();
		
		if (NULL == retVal)
		{
			HARKLE_ERROR(Harklecurse, build_new_cartCoord_struct, allocate_cartCoord_struct failed);
			success = false;
		}
	}
	
	// POPULATE STRUCT
	if (true == success)
	{
		// int absX;								// X coordinate starting at window's top left
		retVal->absX = xVal;
		// int absY;								// Y coordinate starting at window's top left
		retVal->absY = yVal;
		// char graphic;							// Character to print at this coordinate
		retVal->graphic = pntChar;
		// unsigned long hcFlags;					// Implementation-defined coordinate details
		retVal->hcFlags = initStatus;
		// struct hcCartesianCoordinate* nextPnt; 	// Next node in the linked list
		retVal->nextPnt = NULL;
	}
	
	// CLEAN UP
	if (false == success && retVal)
	{
		if (false == free_cartCoord_struct(&retVal))
		{
			HARKLE_ERROR(Harklecurse, build_new_cartCoord_struct, free_cartCoord_struct failed);
		}
	}
	
	// DONE
	return retVal;
}


hcCartCoord_ptr add_cartCoord_node(hcCartCoord_ptr headPnt, hcCartCoord_ptr newPnt, int pntPos)
{
	// LOCAL VARIABLES
	hcCartCoord_ptr retVal = NULL;  // This variable will hold the head node
	hcCartCoord_ptr currNode = NULL;  // This variable will walk the linked list
	bool success = true;  // If anything fails, make this false
	int numNodes = 0;  // Stores the number of nodes in the linked list
	int currPos = 1;  // Keeps track of the node 'index' as we walk the linked list
	
	// INPUT VALIDATION
	if (NULL == newPnt)
	{
		HARKLE_ERROR(Harklecurse, add_cartCoord_node, NULL pointer);
		success = false;
	}
	else if (headPnt == newPnt)
	{
		HARKLE_ERROR(Harklecurse, add_cartCoord_node, headPnt and newPnt are equal);
		success = false;
	}
	else if (pntPos < 0)
	{
		HARKLE_ERROR(Harklecurse, add_cartCoord_node, Invalid pntPos);
		success = false;
	}
	
	// DETERMINE HEAD NODE
	if (true == success)
	{
		if (NULL == headPnt)
		{
			retVal = newPnt;	
		}
		else
		{
			retVal = headPnt;
		}
	}
		
	// INSERT THE NEW NODE
	if (true == success && retVal != newPnt)
	{
		// Count the nodes in the linked list
		numNodes = get_num_cartCoord_nodes(retVal);
		
		if (numNodes < 0)
		{
			HARKLE_ERROR(Harklecurse, add_cartCoord_node, get_num_cartCoord_nodes failed);
			success = false;
		}
		else
		{
			currNode = retVal;
			
			// New node goes at the end
			if (0 == pntPos || pntPos > numNodes)
			{
				// Find the end
				while (NULL != currNode->nextPnt)
				{
					currNode = currNode->nextPnt;
				}
				
				// Insert the node there
				currNode->nextPnt = newPnt;
			}
			// New node goes at the beginning
			else if (1 == pntPos)
			{
				retVal = newPnt;
				newPnt->nextPnt = headPnt;
			}
			else
			{
				while (currPos < (pntPos - 1))
				{
					if (NULL == currNode->nextPnt)
					{
						HARKLE_ERROR(Harklecurse, add_cartCoord_node, get_num_cartCoord_nodes appears to have miscalculated);
						success = false;
						break;
					}
					else
					{
						currNode = currNode->nextPnt;
						currPos++;						
					}
				}

				if (true == success && currNode)
				{
					newPnt->nextPnt = currNode->nextPnt;
					currNode->nextPnt = newPnt;
				}
			}
		}		
	}
	
	// DONE
	if (false == success)
	{
		retVal = NULL;
	}

	return retVal;
}


int get_num_cartCoord_nodes(hcCartCoord_ptr headPnt)
{
	// LOCAL VARIABLES
	int retVal = 0;  // Default return value if any input validation fails
	hcCartCoord_ptr tmp_ptr = NULL;  // Walk the linked list with this variable
	
	// INPUT VALIDATION
	if (headPnt)
	{
		// COUNT NODES
		tmp_ptr = headPnt;

		while (tmp_ptr)
		{
			retVal++;
			tmp_ptr = tmp_ptr->nextPnt;
		}
	}

	// DONE
	return retVal;
}


bool free_cartCoord_struct(hcCartCoord_ptr* oldStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails

	// INPUT VALIDATION
	if (NULL == oldStruct_ptr || NULL == *oldStruct_ptr)
	{
		HARKLE_ERROR(Harklecurse, free_cartCoord_struct, NULL pointer);
		retVal = false;
	}
	else
	{
		// FREE NODE
		// 1. Zeroize/Free/NULL any nodes beneath this one in the linked list
		if (NULL != (*oldStruct_ptr)->nextPnt)
		{
			retVal = free_cartCoord_struct(&((*oldStruct_ptr)->nextPnt));

			if (false == retVal)
			{
				HARKLE_ERROR(Harklecurse, free_cartCoord_struct, Recursive call to free_cartCoord_struct has failed);
			}
		}
		// 2. Zeroize/Free/NULL this node's members
		// int absX;								// X coordinate starting at window's top left
		(*oldStruct_ptr)->absX = 0;
		// int absY;								// Y coordinate starting at window's top left
		(*oldStruct_ptr)->absY = 0;
		// char graphic;							// Character to print at this coordinate
		(*oldStruct_ptr)->graphic = 0;
		// unsigned long hcFlags;					// Implementation-defined coordinate details
		(*oldStruct_ptr)->hcFlags = 0;
		// struct hcCartesianCoordinate* nextPnt;  // Next node in the linked list
		// Handled recursively above (or below, depending on how you look at it... recursively)
		// 3. Free/NULL this node
		free(*oldStruct_ptr);
		*oldStruct_ptr = NULL;
	}
	
	// DONE
	return retVal;
}


bool free_cardCoord_linked_list(hcCartCoord_ptr* oldHeadNode_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Set this to false if anything fails
	
	// INPUT VALIDATION
	if (NULL == oldHeadNode_ptr || NULL == *oldHeadNode_ptr)
	{
		HARKLE_ERROR(Harklecurse, free_cartCoord_struct, NULL pointer);
		retVal = false;
	}
	else
	{
		retVal = free_cartCoord_struct(oldHeadNode_ptr);
	}

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


/*
	PURPOSE - Print all cartCoord nodes into a given ncurses WINDOW
	INPUT
		currWin - Pointer to a WINDOW object
		headNode - Head node to a linked list of hcCartesianCoordinate structs
	OUTPUT
		On success, true
		On failure, false
 */
bool print_plot_list(WINDOW* currWin, hcCartCoord_ptr headNode)
{
	// LOCAL VARIABLES
	bool retVal = true;  // If anything fails, make this false
	hcCartCoord_ptr currNode = NULL;  // Current node being printed

	// INPUT VALIDATION
	if (NULL == currWin || NULL == headNode)
	{
		HARKLE_ERROR(Harklecurse, print_plot_list, NULL pointer);
		retVal = false;
	}
	else
	{
		currNode = headNode;
	}

	// WALK LINKED LIST
	while (currNode && true == retVal)
	{
		// fprintf(stdout, "Printing %c at (%d, %d)\t", currNode->graphic, currNode->absX, currNode->absY);  // DEBUGGING
		if (OK != mvwaddch(currWin, currNode->absY, currNode->absX, currNode->graphic))
		{
			fprintf(stdout, "Failed to print %c at (%d, %d)\t", currNode->graphic, currNode->absX, currNode->absY);  // DEBUGGING
			HARKLE_ERROR(Harklecurse, print_plot_list, mvwaddch failed);
			retVal = false;
		}
		else
		{
			currNode = currNode->nextPnt;
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// NCURSES FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
