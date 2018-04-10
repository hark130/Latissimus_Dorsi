#ifndef __HARKLECURSE__
#define __HARKLECURSE__

#include <ncurses.h>			// WINDOW
#include <stdbool.h>			// bool, true, false

// This struct defines details about an ncurses window
typedef struct hCurseWinDetails
{
	WINDOW* win_ptr;	// ncurses WINDOWS pointer
	int nRows;			// Number of rows
	int nCols;			// Number of columns
	int upperR;			// Upper most row
	int leftC;			// Left most column
} winDetails, *winDetails_ptr;

// This struct defines details about an absolute cartesian coordinate to
//	plot in an ncurses window
typdef struct hcCartesianCoordinate
{
	int absX;								// X coordinate starting at window's top left
	int absY;								// Y coordinate starting at window's top left
	char graphic;							// Character to print at this coordinate
	unsigned long hcFlags;					// Implementation-defined coordinate details
	struct hcCartesianCoordinate* nextPnt;  // Next node in the linked list
} hcCartCoord, *hcCartCoord_ptr;

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
hcCartCoord_ptr allocate_cartCoord_struct(void);


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
hcCartCoord_ptr build_new_cartCoord_struct(int xVal, int yVal, char pntChar, unsigned long initStatus);


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
hcCartCoord_ptr add_cartCoord_node(hcCartCoord_ptr headPnt, hcCartCoord_ptr newPnt, int pntPos);


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
int get_num_cartCoord_nodes(hcCartCoord_ptr headPnt);


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
bool free_cartCoord_struct(hcCartCoord_ptr* oldStruct_ptr);


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
bool free_cardCoord_linked_list(hcCartCoord_ptr* oldHeadNode_ptr);


////////////////////// CARTESIAN COORDINATE STRUCT STOP //////////////////////

///////////////////////// NCURSES WINDOW STRUCT START ////////////////////////


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


///////////////////////// NCURSES WINDOW STRUCT STOP /////////////////////////

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
