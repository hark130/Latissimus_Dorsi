#ifndef __HARKLECURSE__
#define __HARKLECURSE__

#include <ncurses.h>			// WINDOW

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


#endif  // __HARKLECURSE__
