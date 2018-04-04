#include "Harklerror.h"
#include <ncurses.h>
#include <stdbool.h>			// bool, true, false


int kill_a_window(WINDOW** oldWin_ptr)
{
	// LOCAL VARIABLES
	bool success = true;  // Make this false if anything fails
	retVal = OK;  // Function's return value
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
