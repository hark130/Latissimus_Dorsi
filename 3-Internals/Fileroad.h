/*
	This header will be a collection of all my nit-noid file I/O
		functions I've ever written.  Frankly, I'm sick of
		rewriting them every time.
 */

#ifndef __FILEROAD__
#define __FILEROAD__

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// INPUT FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose: Remove any pesky characters lurking around on the stdin stream
	Input - None
	Output - None
	Notes:
		Will read from stdin until an EOF or newline is read
 */
void clear_stdin(void);


/*
	Purpose: Read a number from stdin
	Input - None
	Output - A signed integer
	Notes:
		Calls clear_stdin() after reading to clear the stream
 */
int read_a_num(void);


/*
	Purpose: Read a number from stdin into a heap-allocated char array
	Input - None
	Output
		On success, a heap-allocated char array containing the number read
		On failure, NULL
	Notes:
		Will return NULL if any input is non-character
		Will return NULL if input exceeds buffer length
        It is the caller's responsibility to free the char* returned by 
        	this function
		
 */
char* buff_a_num(void);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// INPUT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __FILEROAD__