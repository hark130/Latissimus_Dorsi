/*
	This header is purely to standardize error output in my projects.
	I don't want to define HARKLE_ERROR() more than one.
	I also don't want to have to comment/uncomment more than one
		DEBUG_ON macro.
 */

#ifndef __HARKLERROR__
#define __HARKLERROR__

// #define HARKLE_DEBUG  // Comment this out to turn off DEBUGGING

#ifdef HARKLE_DEBUG
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#else
#define HARKLE_ERROR(haeder, funcName, ms) ;;;
#endif  // HARKLE_ERROR

#endif  // __HARKLERROR__