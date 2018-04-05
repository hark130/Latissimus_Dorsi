#ifndef __HARKLEPIPE__
#define __HARKLEPIPE__

// MACROs to help properly access int array indices
#define HPIPE_READ 0
#define HPIPE_WRITE 1


/*
	PURPOSE - Make plumbing easy
	INPUT
		emptyPipes - Integer array of dimension 2 to store the resulting file descriptors
		flags - Flags to pass to pipe2() if appropriate
	OUTPUT
		On success, 0
		On failure, errno
	NOTES
		Based on MACROS and the value of flags, make_a_pipe() will call pipe2() instead
			of pipe() in the right circumstances
		This function will return the original values of emptyPipes to their original
			indices in the event of failure... just in case pipe()/pipe2() changed
			something.
 */
int make_a_pipe(int emptyPipes[2], int flags);


#endif  // __HARKLEPIPE__