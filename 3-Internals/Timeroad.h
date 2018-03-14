#ifndef __TIMEROAD__
#define __TIMEROAD__


/*
	Purpose - Translate the local time into a statically allocated tm struct
	Input - None
	Output
		On success, a statically allocated tm struct pointer
		On failure, NULL
	Notes:
		Do NOT free() the return value from this function
 */
struct tm* get_localtime(void);


/*
	Purpose - Translate the current local time into a YYYYMMDD-HHMMSS string format
	Input
		dtStruct - Pointer to a tm struct
	Output
		On succes, heap-allocated, nul-terminated, datetime-stamp formatted string
		On failure, NULL
	Notes:
		This function calls get_localtime()
		It is the caller's responsibility to free the return value
 */
char* build_timestamp(void);


#endif  // __TIMEROAD__