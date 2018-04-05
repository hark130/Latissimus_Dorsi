#ifndef __HARKLETHREAD__
#define __HARKLETHREAD__

#include <pthread.h>

typedef struct hThreadDetails
{
	char* tName;						// Name of the thread (optional)
	int tNum;							// Application-defined number
	pthread_t threadID;					// ID returned by pthread_create()
	void*(*strtFunc)(void*);			// Function point to the thread's startup function
	void* tArgvString;					// From command-line argument
	size_t tArgSize;					// Size of the buffer containing argvString and any nul/NULL termination
	pthread_mutex_t pipeMutex; 			// Thread's pipe mutex
	pthread_mutexattr_t pipeMutexAttr;	// Attributes for thread's pipe mutex
	int pipeFDs[2];						// Pipe used to send data from the thread to the main thread
} hThrDetails, *hThrDetails_ptr;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	PURPOSE - Do the heavy lifting for hThreadDetails struct initialization
	INPUT
		threadName [optional] - nul-terminated char string to use as a name
		threadNum [optional] - Application-implemented numbering system
		start_routine - Thread's startup function
		arg - Command line arguments to copy in
		argSize - The number of bytes to allocate in order to copy arg into
			a buffer 'owned' by this new struct.  Ensure argSize is large
			enough to account for any nul/NULL termination include in arg.
	NOTES
		This function calls:
			Memoroad's copy_a_string() it threadName is not NULL
			allocate_a_hThrDetails_ptr() to allocate heap memory
			Harklepipe's make_a_pipe() to initialize pipeFDs
			_____() to initialize the mutex
		This function does NOT call pthread_create().  The caller should:
			pthread_create(&(self->threadID), NULL, start_routine, argvString);
		arg's pointer will not be copied into the struct.  Instead, arg's
			buffer will be replicated into a heap-allocated array stored in
			self->argvString, dimension of argSize. 
 */
hThrDetails_ptr create_a_hThrDetails_ptr(char* threadName, \
	                                     int threadNum, \
	                                     void* (*start_routine) (void*), \
	                                     void* arg, \
	                                     size_t argSize);


/*
	PURPOSE - Allocate heap memory for a hCurseWinDetails struct pointer
	INPUT - None
	OUTPUT
		On success, a pointer to a hCurseWinDetails struct
		On failure, NULL
	NOTES
		It is the caller's responsibility to free the memory allocated here
 */
hThrDetails_ptr allocate_a_hThrDetails_ptr(void);


/*
	PURPOSE - Zeroize, free, and NULL a heap-allocated hCurseWinDetails struct pointer
	INPUT
		oldStruct_ptr - A pointer to a heap-allocated hThrDetails_ptr
	OUTPUT
		On success, true
		On failure, false
	NOTES
		The following struct members will be zeroized, free()d, destroyed, etc:
			name
			threadNum
			threadID(?)
			argvString
			argSize
			pipeMutex
			pipeMutexAttr
			pipeFDs
		The variable pointed at by oldStruct_ptr will be assigned NULL
 */
bool free_a_hThrDetails_ptr(hThrDetails_ptr* oldStruct_ptr);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// STRUCT FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// PTHREAD FUNCTIONS START //////////////////////////
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
/////////////////////////// PTHREAD FUNCTIONS STOP ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#endif  // __HARKLETHREAD__
