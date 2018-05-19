/*
	REPO:		Latissiumus_Dorsi (https://github.com/hark130/Latissimus_Dorsi)
	FILE:		Harklerror.h
	PURPOSE:	Define conditionally-compiled debugging MACROS
	DATE:		Updated 20180518
	VERSION:	0.2.0
 */

#include <stdio.h>
#include <string.h>							// strerror()

#ifndef __HARKLERROR__
#define __HARKLERROR__

#define HARKLE_DEBUG  // Comment this out to turn off DEBUGGING MACROS

#ifdef HARKLE_DEBUG
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#define HARKLE_ERRNO(header, funcName, errorNum) if (errno) { fprintf(stderr, "<<<ERROR>>> - %s - %s() returned errno:\t%s\n", #header, #funcName, strerror(errorNum)); }
#define HARKLE_WARNG(header, funcName, msg) do { fprintf(stderr, "¿¿¿WARNING??? - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#else
#define HARKLE_ERROR(header, funcName, msg) ;;;
#define HARKLE_ERRNO(header, funcName, msg) ;;;
#define HARKLE_WARNG(header, funcName, msg) ;;;
#endif  // HARKLE_DEBUG

#endif  // __HARKLERROR__

/*
	CHANGE LOG:
		v0.1.0
			- Initial commit of HARKLE_ERROR
		v0.2.0
			- Added HARKLE_ERRNO
			- Added HARKLE_WARNG
 */
