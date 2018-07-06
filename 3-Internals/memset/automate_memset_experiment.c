/*
 *	The purpose of this file is to automate an experiment to determine which conditions will
 *	optimize-out function calls to memset.
 *	https://github.com/hark130/Latissimus_Dorsi/tree/memset/3-Internals/memset
 */

#include "Harklerror.h"							// HARKLE_ERROR
#include <stdbool.h>							// bool, true, false
#include "Timeroad.h"							// build_timestamp()

/*
 *	COMBINATORIAL INPUT MACROS
 *  memset-<THING><TRICK><OBJECT><SCHEME><OPTIMIZATION LEVEL>.exe
 */
// Thing - A list of different "things" to attempt to memset
#define THING_UPPER_LIMIT 4  // Update this if you add another thing
#define THING1 "Local scope"
#define THING2 "Global scope"
#define THING3 "Heap memory"
#define THING4 "mmap() memory"
// Trick - A list of "tricks" to attempt (e.g., volatile, explicit_bzero)
#define TRICK_UPPER_LIMIT 7  // Update this if you add another trick
#define TRICK1 "None"
#define TRICK2 "Volatile"
#define TRICK3 "pragma"
#define TRICK4 "memset_s()"
#define TRICK5 "pass to do-nothing func"
#define TRICK6 "read/write/replace 	"
#define TRICK7 "explicit_bzero"
// Object - A list of "objects" to call memset (e.g., function, goto, inline assembly)
#define OBJECT_UPPER_LIMIT 3  // Update this if you add another object
#define OBJECT1 "Function"
#define OBJECT2 "Goto"
#define OBJECT3 "Inline Assembly"
// Scheme - A list of compilation schemes to attempt (e.g., linked object code, shared object)
#define SCHEME_UPPER_LIMIT 5  // Update this if you add another scheme
#define SCHEME1 "main()"
#define SCHEME2 "Local func()"
#define SCHEME3 "Compiled Together"
#define SCHEME4 "Linked Together"
#define SCHEME5 "Shared Object"
// Optimization
#define OPTIMIZATION_UPPER_LIMIT 4
#define OPTIMIZATION0 "None"
#define OPTIMIZATION1 "-O1"
#define OPTIMIZATION2 "-O2"
#define OPTIMIZATION3 "-O3"

/*
 *	ENVIRONMENT MACROS
 */
#define INPUT_FILE_PATH ""	// Currently, the input files are in the local directory
#define INPUT_FILE_TEMPLATE "memset-00000.exe"  // This will be updated as tests iterate

/*
 *	OUTPUT MACROS
 */
#define OUTPUT_MEMSET_FOUND 	"Found"
#define OUTPUT_MEMSET_MISSING	"Absent"
#define OUTPUT_FILE_MISSING 	"No File"


/*
 *	PURPOSE - Automate the update of the templateFname based on combinatorial input
 *	NOTES
 *		Hard-coded to update memset-00000 (file extension not included)
 *		templateFname will be modified in-place
 */
bool update_filename(char* templateFname, int nThing, int nTrick, int nObj, int nScheme, int nOpt);


int main(void)
{
	// LOCAL VARIABLES
	int retVal = 0;
	bool success = true;
	int numThings = THING_UPPER_LIMIT;
	int numTricks = TRICK_UPPER_LIMIT;
	int numObjects = OBJECT_UPPER_LIMIT;
	int numSchemes = SCHEME_UPPER_LIMIT;
	int numOpts = OPTIMIZATION_UPPER_LIMIT;
	char tempFilename[] = { INPUT_FILE_PATH INPUT_FILE_TEMPLATE };  // Template input filename
	char logFilename[] = { "YYYYMMDD-HHMMSS_memset_results.md" };  // Log filename
	char *tmp_ptr = NULL;

	// INPUT VALIDATION
	if (numTricks < 1 || numTricks < 1 || numObjects < 1 || numSchemes < 1 || numOpts < 1)
	{
		HARKLE_ERROR(automate_memset_experiment, main, Invalid value);
		success = false;
	}

	// BEGIN
	// 1. Setup (.md log file)
	if (true == success)
	{
		// Get timestamp

		// Update logFilename

		// Free timestamp

		// Create file
		// Existing Harklemodule?
	}

	// Loop Input
	// 	2. Update input filename
	// 	3. Map input filename
	// 	4. Parse mapped input file
	// 	5. Print results to stdout
	// 	6. Unmap input filename
	// 7. Clean up

	// DONE
	return retVal;
}


bool update_filename(char* templateFname, int nThing, int nTrick, int nObj, int nScheme, int nOpt)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char tempFnameStr[] = { "memset-" };
	size_t tempFnameStrLen = sizeof(tempFnameStr) / sizeof(*tempFnameStr);
	char *tmp_ptr = NULL;  // Store return values here
	int errNum = 0;  // Store errno here

	// INPUT VALIDATION
	if (!templateFname)
	{
		HARKLE_ERROR(automate_memset_experiment, update_filename, NULL pointer);
		retVal = false;
	}
	else if (!(*templateFname))
	{
		HARKLE_ERROR(automate_memset_experiment, update_filename, Empty string);
		retVal = false;
	}
	else if (strlen(templateFname) < (tempFnameStrLen + 5))
	{
		HARKLE_ERROR(automate_memset_experiment, update_filename, String too short);
		retVal = false;
	}
	else if (nThing < 0 || nTrick < 0 || nObj < 0 || nScheme < 0 || nOpt < 0)
	{
		HARKLE_ERROR(automate_memset_experiment, update_filename, nth value too small);
		retVal = false;
	}
	else if (nThing > 9 || nTrick > 9 || nObj > 9 || nScheme > 9 || nOpt > 9)
	{
		HARKLE_ERROR(automate_memset_experiment, update_filename, nth value too big);
		retVal = false;
	}

	// UPDATE
	if (true == retVal)
	{
		// Find the substring
		tmp_ptr = strstr(templateFname, tempFnameStr);

		if (!tmp_ptr)
		{
			// ERRNO
			HARKLE_ERROR(automate_memset_experiment, update_filename, strstr failed);
			retVal = false;
		}
	}

	// DONE
	return retVal;
}
