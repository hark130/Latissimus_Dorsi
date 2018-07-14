/*
 *	The purpose of this file is to automate an experiment to determine which conditions will
 *	optimize-out function calls to memset.
 *	https://github.com/hark130/Latissimus_Dorsi/tree/memset/3-Internals/memset
 */

#include <errno.h>								// errno
#include <fcntl.h>								// open() flags
#include "Elf_Manipulation.h"					// search_sect_hdr64_und_func()
#include "Fileroad.h"							// os_path_isfile()
#include "Harklerror.h"							// HARKLE_ERROR
#include "Map_Memory.h"							// map_file_mode(), unmap_file(), free_struct()
#include "Memoroad.h"							// release_a_string()
#include <stdbool.h>							// bool, true, false
#include <stdio.h>								// sprintf(), fopen()
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
#define TRICK_UPPER_LIMIT 8  // Update this if you add another trick
#define TRICK1 "None"
#define TRICK2 "Volatile"
#define TRICK3 "pragma"
#define TRICK4 "memset_s()"
#define TRICK5 "pass to do-nothing func"
#define TRICK6 "read/write/replace 	"
#define TRICK7 "explicit_bzero"
#define TRICK8 "touching memory"
// Object - A list of "objects" to call memset (e.g., function, goto, inline assembly)
#define OBJECT_UPPER_LIMIT 3  // Update this if you add another object
#define OBJECT1 "Function"
#define OBJECT2 "Goto"
#define OBJECT3 "Inline Assembly"
// Scheme - A list of compilation schemes to attempt (e.g., linked object code, shared object)
#define SCHEME_UPPER_LIMIT 4  // Update this if you add another scheme
#define SCHEME1 "main()"
#define SCHEME2 "Local func()"
#define SCHEME3 "Compiled Together"
#define SCHEME4 "Linked Together"
#define SCHEME5 "Shared Object"  // memset will only appear in the .so's dynsym.  Hard code them.
// Optimization
#define OPTIMIZATION_UPPER_LIMIT 4  // Update this if you support another level of optimization
#define OPTIMIZATION0 "None"
#define OPTIMIZATION1 "-O1"
#define OPTIMIZATION2 "-O2"
#define OPTIMIZATION3 "-O3"
// General Use
#define UNDEFINED "Undefined"
#define NOT_APPLICABLE "N/A"
#define INDETERMINATE "Indeterminate"

/*
 *	ENVIRONMENT MACROS
 */
#define INPUT_FILE_PATH ""	// Currently, the input files are in the local directory
#define INPUT_FILE_TEMPLATE "memset-00000.exe"  // This will be updated as tests iterate

/*
 *	OUTPUT MACROS
 */
#define OUTPUT_MEMSET_FOUND 	"memset Found"
#define OUTPUT_MEMSET_MISSING	"memset Absent"
#define OUTPUT_FILE_MISSING 	"No Source File"
#define LOG_COL_1 "Filename"
#define LOG_COL_2 "Thing"
#define LOG_COL_3 "Trick"
#define LOG_COL_4 "Object"
#define LOG_COL_5 "Scheme"
#define LOG_COL_6 "Optimization"
#define LOG_COL_7 "Results"


/*
 *	PURPOSE - Automate the update of the templateFname based on combinatorial input
 *	NOTES
 *		Hard-coded to update memset-00000 (file extension not included)
 *		templateFname will be modified in-place
 */
bool update_filename(char* templateFname, int nThing, int nTrick, int nObj, int nScheme, int nOpt);


/*
 *	PURPOSE - Log gathered information into a pre-opened .md formatted log file pointer
 *	NOTES
 *		Column alignment is hard-coded inside this function
 *		If header is true, this function will automatically print a line to align the columns
 */
bool log_exp_entry(FILE *log_ptr, const char *col1, const char *col2, const char *col3, \
	               const char *col4, const char *col5, const char *col6, const char *col7, \
	               bool header);


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
	char resultsLogFilename[] = { "YYYYMMDD-HHMMSS_memset_results.md" };  // Filename of the log with all the results
	char successLogFilename[] = { "YYYYMMDD-HHMMSS_memset_success.md" };  // Filename of the log containing just the "found" results
	char *tmp_ptr = NULL;  // Store return values here
	int errNum = 0;  // Store errno here
	errno = 0;
	FILE *resultsLogFile = NULL;  // Opened full results log file
	FILE *successLogFile = NULL;  // Opened just success log file
	int nthThing = 1;  // Starting thing number
	int nthTrick = 1;  // Starting trick number
	int nthObject = 1;  // Starting object number
	int nthScheme = 1;  // Starting scheme number
	int nthOptim = 0;  // Starting optimization number
	mapMem_ptr mapInFile_ptr = NULL;  // map_file_mode() input files here
	// Update the appropriate array if a new MACRO was added
	char *thing_arr[THING_UPPER_LIMIT + 1] = { NULL, THING1, THING2, THING3, THING4 };
	char *trick_arr[TRICK_UPPER_LIMIT + 1] = { NULL, TRICK1, TRICK2, TRICK3, TRICK4, TRICK5, TRICK6, TRICK7, TRICK8 };
	char *object_arr[OBJECT_UPPER_LIMIT + 1] = { NULL, OBJECT1, OBJECT2, OBJECT3 };
	char *scheme_arr[SCHEME_UPPER_LIMIT + 1] = { NULL, SCHEME1, SCHEME2, SCHEME3, SCHEME4 };
	char *optim_arr[OPTIMIZATION_UPPER_LIMIT + 1] = { OPTIMIZATION0, OPTIMIZATION1, OPTIMIZATION2, OPTIMIZATION3 };
	char *manualEntries_arr[] = { "libhset.so", "libhset0.so", \
	                              "libhset1.so", "libhset2.so", \
	                              "libhset3.so", \
	                              NULL };

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
		tmp_ptr = build_timestamp();

		if (!tmp_ptr)
		{
			HARKLE_ERROR(automate_memset_experiment, main, build_timestamp failed);
			success = false;
		}

		// Update resultsLogFilename
		if (true == success)
		{
			if (resultsLogFilename != memcpy(resultsLogFilename, tmp_ptr, strlen(tmp_ptr)))
			{
				errNum = errno;
				HARKLE_ERROR(automate_memset_experiment, main, memcpy failed);
				HARKLE_ERRNO(automate_memset_experiment, memcpy, errNum);
				retVal = false;
			}
		}

		// Update successLogFilename
		if (true == success)
		{
			if (successLogFilename != memcpy(successLogFilename, tmp_ptr, strlen(tmp_ptr)))
			{
				errNum = errno;
				HARKLE_ERROR(automate_memset_experiment, main, memcpy failed);
				HARKLE_ERRNO(automate_memset_experiment, memcpy, errNum);
				retVal = false;
			}
		}

		// Free timestamp
		if (tmp_ptr)
		{
			if (false == release_a_string(&tmp_ptr))
			{
				HARKLE_ERROR(automate_memset_experiment, main, release_a_string failed);
				retVal = false;
				tmp_ptr = NULL;
			}
		}

		// Create file
		// resultsLogFilename
		if (true == success)
		{
			puts(resultsLogFilename);  // DEBUGGING
			resultsLogFile = fopen(resultsLogFilename, "w");

			if (!resultsLogFile)
			{
				errNum = errno;
				HARKLE_ERROR(automate_memset_experiment, main, fopen failed);
				HARKLE_ERRNO(automate_memset_experiment, fopen, errNum);
				success = false;
			}
			else
			{
				if (false == log_exp_entry(resultsLogFile, LOG_COL_1, LOG_COL_2, LOG_COL_3, LOG_COL_4, \
					                       LOG_COL_5, LOG_COL_6, LOG_COL_7, true))
				{
					HARKLE_ERROR(automate_memset_experiment, main, fopen failed);
					success = false;
				}
			}			
		}
		// successLogFilename
		if (true == success)
		{
			puts(successLogFilename);  // DEBUGGING
			successLogFile = fopen(successLogFilename, "w");

			if (!successLogFile)
			{
				errNum = errno;
				HARKLE_ERROR(automate_memset_experiment, main, fopen failed);
				HARKLE_ERRNO(automate_memset_experiment, fopen, errNum);
				success = false;
			}
			else
			{
				if (false == log_exp_entry(successLogFile, LOG_COL_1, LOG_COL_2, LOG_COL_3, LOG_COL_4, \
					                       LOG_COL_5, LOG_COL_6, LOG_COL_7, true))
				{
					HARKLE_ERROR(automate_memset_experiment, main, fopen failed);
					success = false;
				}
			}			
		}
	}

	// Loop Input
	for (int i = nthThing; i <= numThings && true == success; i++)
	{
		for (int j = nthTrick; j <= numTricks && true == success; j++)
		{
			for (int k = nthObject; k <= numObjects && true == success; k++)
			{
				for (int l = nthScheme; l <= numSchemes && true == success; l++)
				{
					for (int m = nthOptim; m < numOpts && true == success; m++)
					{
						// 2. Update input filename
						if (false == update_filename(tempFilename, i, j, k, l, m))
						{
							HARKLE_ERROR(automate_memset_experiment, main, update_filename failed);
							success = false;
						}
						// 3. Does the file exist?
						if (true == success)
						{
							if (false == os_path_isfile(tempFilename))
							{
								// 6. Print results to stdout
								// fprintf(stdout, "%s:\tDoes not exist\n", tempFilename);

								// 7. Log the results
								log_exp_entry(resultsLogFile, tempFilename, thing_arr[i], trick_arr[j], \
									          object_arr[k], scheme_arr[l], optim_arr[m], \
									          OUTPUT_FILE_MISSING, false);
							}
							// 4. Map input filename
							else
							{
								mapInFile_ptr = map_file_mode(tempFilename, O_RDONLY);

								if (!mapInFile_ptr)
								{
									// 6. Print results to stdout
									fprintf(stdout, "%s:\tUnable to map to memory\n", tempFilename);
								}
								else
								{								
									// 5. Parse mapped input file
									if (true == search_sect_hdr64_und_func(mapInFile_ptr, "memset"))
									{
										// 6. Print results to stdout
										fprintf(stdout, "%s:\t%s\n", tempFilename, OUTPUT_MEMSET_FOUND);

										// 7. Log the results
										log_exp_entry(resultsLogFile, tempFilename, thing_arr[i], trick_arr[j], \
											          object_arr[k], scheme_arr[l], optim_arr[m], \
											          OUTPUT_MEMSET_FOUND, false);
										log_exp_entry(successLogFile, tempFilename, thing_arr[i], trick_arr[j], \
											          object_arr[k], scheme_arr[l], optim_arr[m], \
											          OUTPUT_MEMSET_FOUND, false);
									}
									else
									{
										// 6. Print results to stdout
										fprintf(stdout, "%s:\t%s\n", tempFilename, OUTPUT_MEMSET_MISSING);

										// 7. Log the results
										log_exp_entry(resultsLogFile, tempFilename, thing_arr[i], trick_arr[j], \
											          object_arr[k], scheme_arr[l], optim_arr[m], \
											          OUTPUT_MEMSET_MISSING, false);
									}

									// 8. Unmap input filename
									if (mapInFile_ptr)
									{
										// Unmap the memory
										if (false == unmap_file(mapInFile_ptr, false))
										{
											HARKLE_ERROR(automate_memset_experiment, main, unmap_file failed);
											success = false;
										}
										// Free the struct pointer
										free_struct(&mapInFile_ptr);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// 9. Check libraries
	for (size_t i = 0; i < (sizeof(manualEntries_arr) / sizeof(*manualEntries_arr)); i++)
	{
		// 2. Get the library name
		tmp_ptr = (*(manualEntries_arr + i));
		// Previous array implementation was NULL-terminated to facilitate while loop
		if (!tmp_ptr)
		{
			break;
		}
		// 3. Does the file exist?
		if (false == os_path_isfile(tmp_ptr))
		{
			// 6. Print results to stdout
			// fprintf(stdout, "%s:\tDoes not exist\n", tmp_ptr);

			// 7. Log the results
			log_exp_entry(resultsLogFile, tmp_ptr, NOT_APPLICABLE, UNDEFINED, \
				          OBJECT1, SCHEME5, INDETERMINATE, \
				          OUTPUT_FILE_MISSING, false);
		}
		// 4. Map input filename
		else
		{
			mapInFile_ptr = map_file_mode(tmp_ptr, O_RDONLY);

			if (!mapInFile_ptr)
			{
				// 6. Print results to stdout
				fprintf(stdout, "%s:\tUnable to map to memory\n", tmp_ptr);
			}
			else
			{								
				// 5. Parse mapped input file
				if (true == search_sect_hdr64_und_func(mapInFile_ptr, "memset"))
				{
					// 6. Print results to stdout
					fprintf(stdout, "%s:\t%s\n", tmp_ptr, OUTPUT_MEMSET_FOUND);

					// 7. Log the results
					log_exp_entry(resultsLogFile, tmp_ptr, NOT_APPLICABLE, UNDEFINED, \
				          		  OBJECT1, SCHEME5, INDETERMINATE, \
						          OUTPUT_MEMSET_FOUND, false);
					log_exp_entry(successLogFile, tmp_ptr, NOT_APPLICABLE, UNDEFINED, \
						          OBJECT1, SCHEME5, INDETERMINATE, \
						          OUTPUT_MEMSET_FOUND, false);
				}
				else
				{
					// 6. Print results to stdout
					fprintf(stdout, "%s:\t%s\n", tmp_ptr, OUTPUT_MEMSET_MISSING);

					// 7. Log the results
					log_exp_entry(resultsLogFile, tmp_ptr, NOT_APPLICABLE, UNDEFINED, \
						          OBJECT1, SCHEME5, INDETERMINATE, \
						          OUTPUT_MEMSET_MISSING, false);
				}

				// 8. Unmap input filename
				if (mapInFile_ptr)
				{
					// Unmap the memory
					if (false == unmap_file(mapInFile_ptr, false))
					{
						HARKLE_ERROR(automate_memset_experiment, main, unmap_file failed);
						success = false;
					}
					// Free the struct pointer
					free_struct(&mapInFile_ptr);
				}
			}
		}
	}

	// 10. Clean up
	// resultsLogFile
	if (resultsLogFile)
	{
		if (EOF == fclose(resultsLogFile))
		{
			errNum = errno;
			HARKLE_ERROR(automate_memset_experiment, main, fclose failed);
			HARKLE_ERRNO(automate_memset_experiment, fclose, errNum);
		}
		resultsLogFile = NULL;
	}
	// successLogFile
	if (successLogFile)
	{
		if (EOF == fclose(successLogFile))
		{
			errNum = errno;
			HARKLE_ERROR(automate_memset_experiment, main, fclose failed);
			HARKLE_ERRNO(automate_memset_experiment, fclose, errNum);
		}
		successLogFile = NULL;
	}
	// mapInFile_ptr
	if (mapInFile_ptr)
	{
		// Unmap the memory
		if (false == unmap_file(mapInFile_ptr, false))
		{
			HARKLE_ERROR(automate_memset_experiment, main, unmap_file failed);
		}
		// Free the struct pointer
		free_struct(&mapInFile_ptr);
	}

	// DONE
	return retVal;
}


bool update_filename(char* templateFname, int nThing, int nTrick, int nObj, int nScheme, int nOpt)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char tempFnameStr[] = { "memset-" };
	char tempNumStr[] = { "00000" };
	size_t tempFnameStrLen = (sizeof(tempFnameStr) / sizeof(*tempFnameStr)) - 1;
	size_t tempNumStrLen = (sizeof(tempNumStr) / sizeof(*tempNumStr)) - 1;
	char *tmp_ptr = NULL;  // Store return values here
	int tmpInt = 0;  // Store return values here
	int errNum = 0;  // Store errno here
	errno = 0;

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
	else if (strlen(templateFname) < (tempFnameStrLen + tempNumStrLen))
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
			errNum = errno;
			HARKLE_ERROR(automate_memset_experiment, update_filename, strstr failed);
			HARKLE_ERRNO(automate_memset_experiment, strstr, errNum);
			retVal = false;
		}
		else
		{
			tmp_ptr += tempFnameStrLen;  // Advance the pointer past the "needle" str

			tmpInt = sprintf(tempNumStr, "%1d%1d%1d%1d%1d", nThing, nTrick, nObj, nScheme, nOpt);

			if (tmpInt != tempNumStrLen)
			{
				errNum = errno;
				HARKLE_ERROR(automate_memset_experiment, update_filename, sprintf failed);
				HARKLE_ERRNO(automate_memset_experiment, sprintf, errNum);
				retVal = false;
			}
			else
			{
				if (tmp_ptr != memcpy(tmp_ptr, tempNumStr, tempNumStrLen * sizeof(*tempNumStr)))
				{
					errNum = errno;
					HARKLE_ERROR(automate_memset_experiment, update_filename, sprintf failed);
					HARKLE_ERRNO(automate_memset_experiment, memcpy, errNum);
					retVal = false;
				}
			}
		}
	}

	// DONE
	return retVal;
}


bool log_exp_entry(FILE *log_ptr, const char *col1, const char *col2, const char *col3, \
	               const char *col4, const char *col5, const char *col6, const char *col7, \
	               bool header)
{
	// LOCAL VARIABLES
	bool retVal = true;
	int errNum = 0;  // Store errno here
	errno = 0;

	// INPUT VALIDATION
	if (!log_ptr || !col1 || !col2 || !col3 || !col4 || !col5 || !col6 || !col7)
	{
		HARKLE_ERROR(automate_memset_experiment, log_exp_entry, NULL pointer);
		retVal = false;
	}
	else if (!(*col1) || !(*col2) || !(*col3) || !(*col4) || !(*col5) || !(*col6) || !(*col7))
	{
		// fprintf(stdout, "Col1:\t%s\nCol2:\t%s\nCol3:\t%s\nCol4:\t%s\nCol5:\t%s\nCol6:\t%s\nCol7:\t%s\n", col1, col2, col3, col4, col5, col6, col7);  // DEBUGGING
		HARKLE_ERROR(automate_memset_experiment, log_exp_entry, Empty string);
		retVal = false;
	}

	// LOG IT
	if (true == retVal)
	{
		if (1 > fprintf(log_ptr, "| %s | %s | %s | %s | %s | %s | %s |\n", \
			            col1, col2, col3, col4, col5, col6, col7))
		{
			errNum = errno;
			HARKLE_ERROR(automate_memset_experiment, log_exp_entry, fprintf failed);
			HARKLE_ERRNO(automate_memset_experiment, fprintf, errNum);
			retVal = false;
		}
		else
		{
			if (true == header)
			{
				if (1 > fprintf(log_ptr, "| :- | :-: | :-: | :-: | :-: | :-: | :-: |\n"))
				{
					errNum = errno;
					HARKLE_ERROR(automate_memset_experiment, log_exp_entry, fprintf failed);
					HARKLE_ERRNO(automate_memset_experiment, fprintf, errNum);
					retVal = false;
				}
			}
		}
	}

	// DONE
	return retVal;
}
