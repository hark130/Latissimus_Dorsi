#ifndef __SIGNALEROAD__
#define __SIGNALEROAD__


/*
	Purpose - New and improved strsignal()
	Input
		sigRoad - signal number
	Output
		On success, heap-allocated string containing both the signal name
			and the strsig()
	Notes:
		The caller is responsible for free()ing the returned string
 */
char* str_signaleroad(int sigRoad);


#endif  // __SIGNALEROAD__