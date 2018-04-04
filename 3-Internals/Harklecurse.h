#ifndef __HARKLECURSE__
#define __HARKLECURSE__


/*
	PURPOSE - Erase an ncurses window
	INPUT
		oldWin_ptr - Pointer to a WINDOW*
	OUTPUT - None
	NOTES:
		This function will zeroize the original pointer variable... for safety 
 */
void kill_a_window(WINDOW** oldWin_ptr);


#endif  // __HARKLECURSE__
