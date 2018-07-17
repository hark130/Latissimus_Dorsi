#include <errno.h>							// errno
#include "Harkledir.h"
#include "Harkleproc.h"
// #include <fcntl.h>	  					// open() flags
#include "Fileroad.h"   					// read_a_file
#include "Harklerror.h"						// HARKLE_ERROR
// #include "Map_Memory.h"
#include <inttypes.h>						// strtoimax()
#include "Memoroad.h"   					// copy_a_string
#include <stdbool.h>						// bool, true, false
#include <stdio.h>
#include <stdlib.h>	 						// calloc
#include <string.h>	 						// strlen, strstr
#include <unistd.h>	 						// read

#ifndef HPROC_MAX_TRIES
// MACRO to limit repeated allocation attempts
#define HPROC_MAX_TRIES 3
#endif  // HPROC_MAX_TRIES

#define HP_PID_BUFF 10

#ifndef HARKLE_ERROR
#define HARKLE_ERROR(header, funcName, msg) do { fprintf(stderr, "<<<ERROR>>> - %s - %s() - %s!\n", #header, #funcName, #msg); } while (0);
#endif  // HARKLE_ERROR

/*
	Purpose - Format buf into /proc/<PID>/ based on PID
	Input
		buf - [OUT] char array to build absolute path
		PID - <PID> in the above format
		bufSize - Size of buf... for safety
	Output - True on success, false on failure
	Notes:
		buf will be memset before work begins
 */
bool proc_builder(char* buf, char* PID, size_t bufSize);


/*
	PURPOSE - Translate a PID string into a pid_t value
	INPUT
		PID - Nul-terminated string representing a PID
	OUTPUT
		On success, pid_t value based on PID
		On failure, 0
 */
pid_t convert_PID(char* PID);


pidDetails_ptr create_PID_struct(void)
{
	// LOCAL VARIABLES
	pidDetails_ptr retVal = NULL;
	int numTries = 0;

	// ALLOCATE
	while (!retVal && numTries < HPROC_MAX_TRIES)
	{
		retVal = (pidDetails_ptr)calloc(1, sizeof(pidDetails));
		numTries++;
	}

	if (!retVal)
	{
		HARKLE_ERROR(Harkleproc, create_PID_struct, Failed to allocate a pidDetails struct pointer);
	}

	// DONE
	return retVal;
}


pidDetails_ptr populate_PID_struct(char* pidPath)
{
	// LOCAL VARIABLES
	pidDetails_ptr retVal = NULL;
	int errNum = 0;  // Capture errno here
	size_t pathLen = 0;  // strlen of pidPath
	char* newPIDPath = NULL;  // In case we need to add a trailing slash
	char* temp_ptr = NULL;  // Return value from string.h functions
	char* pidCommandline = NULL;  // Temporarily holds the absolute path to /proc/<PID>/cmdline
	size_t cmdlineLen = 0;  // The strlen of pidCommandline
	int numTries = 0;  // Check this against nax number calloc attempts
	bool success = true;  // If this is false prior to return, clean up
	char pidNumber[HP_PID_BUFF + 1] = { 0 };  // Use this to extract and convert the PID
	int i = 0;  // Iterating variable

	// 1. INPUT VALIDATION
	if (pidPath)
	{
		if (*pidPath)
		{
			pathLen = strlen(pidPath);
			// fprintf(stdout, "pidPath %s is %zu characters long\n", pidPath, pathLen);  // DEBUGGING
			// fprintf(stdout, "%s is %zu characters long\n", "/proc/1/", strlen("/proc/1/"));  // DEBUGGING

			// 1.1. Minimum length of a /proc/<PID> directory
			if (pathLen >= strlen("/proc/1/"))
			{
				if ((*(pidPath + pathLen - 1)) != '/')
				{
					// We want trailing slashes
					// 1.1.1. Allocate a new array
					while (!newPIDPath && numTries < HPROC_MAX_TRIES)
					{
						// pathLen for the original path
						// 1 for the new trailing slash
						// 1 for the new nul terminator
						newPIDPath = (char*)calloc(pathLen + 2, sizeof(char));
						numTries++;
					}

					if (newPIDPath)
					{
						// 1.1.2. Copy the old path in
						temp_ptr = strncpy(newPIDPath, pidPath, pathLen);

						if (temp_ptr == newPIDPath)
						{
							// 1.1.3. Add a trailing slash
							temp_ptr = NULL;  // Don't need this anymore
							(*(newPIDPath + pathLen)) = '/';
							(*(newPIDPath + pathLen + 1)) = '\0';  // ...for safety
							pathLen = strlen(newPIDPath);
						}
						else
						{
							HARKLE_ERROR(Harkleproc, populate_PID_struct, strncpy failed);
							success = false;
						}
					}
					else
					{
						HARKLE_ERROR(Harkleproc, populate_PID_struct, calloc failed);
						success = false;
					}
				}
				else
				{
					newPIDPath = pidPath;
				}
			}
			else
			{
				HARKLE_ERROR(Harkleproc, populate_PID_struct, Path too short);
				success = false;
			}

			// 1.2. Begins with /proc/
			if (success == true)
			{
				temp_ptr = strstr(newPIDPath, "/proc/");

				if (temp_ptr != newPIDPath)
				{
					HARKLE_ERROR(Harkleproc, populate_PID_struct, Not a /proc path);
					success = false;
				}
			}

			// 1.3. Numerical directory name
			if (success == true)
			{

				temp_ptr = newPIDPath + strlen("/proc/");

				while (*temp_ptr != '/' && success == true)
				{
					if (*temp_ptr < 48 || *temp_ptr > 57)
					{
						// Found a non-number
						HARKLE_ERROR(Harkleproc, populate_PID_struct, Not a /proc/<PID> path);
						success = false;
					}
					else if (*temp_ptr == '\0')
					{
						// Missed the trailing slash?!
						HARKLE_ERROR(Harkleproc, populate_PID_struct, Not a /proc/<PID> path);
						success = false;
					}
					else
					{
						temp_ptr++;
					}
				}
			}
		}
		else
		{
			HARKLE_ERROR(Harkleproc, populate_PID_struct, Empty string);
			success = false;
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, populate_PID_struct, NULL pointer);
		success = false;
	}

	// 2. POPULATE STRUCT
	if (success == true)
	{
		// 2.1. Allocate struct
		retVal = create_PID_struct();

		// 2.2. Assign values
		if (retVal)
		{
			// 2.2.1. Assign pidNum
			// 2.2.1.1. Copy the PID portion into pidNumber
			temp_ptr = newPIDPath + strlen("/proc/");
			if (pidNumber != strncpy(pidNumber, temp_ptr, HP_PID_BUFF))
			{
				HARKLE_ERROR(Harkleproc, populate_PID_struct, strncpy failed);
				success = false;
			}
			else
			{
				for (i = 0; i < HP_PID_BUFF; i++)
				{
					if ('/' == pidNumber[i])
					{
						pidNumber[i] = '\0';  // Terminate the trailing slash
						break;
					}
				}
				// pid_t pidNum;			// PID number
				retVal->pidNum = convert_PID(pidNumber);

				if (0 == retVal->pidNum)
				{
					errNum = errno;
					HARKLE_ERROR(Harkleproc, populate_PID_struct, convert_PID failed);
					if (errNum)
					{
						fprintf(stderr, "convert_PID() returned errno:\t%s\n", strerror(errNum));  // DEBUGGING
					}
					success = false;
				}
			}

			// 2.2.2. Copy pidName
			retVal->pidName = copy_a_string(newPIDPath);

			if (retVal->pidName)
			{
				numTries = 0;

				// 2.2.3. Read/allocate/copy /proc/<PID>/cmdline into char* pidCmdline
				while (!pidCommandline && numTries < HPROC_MAX_TRIES)
				{
					// pathLen for /proc/<PID>/
					// sizeof("cmdline") to make room for the filename
					cmdlineLen = pathLen + sizeof("cmdline");
					// 1 for the new nul-terminating character
					pidCommandline = (char*)calloc(cmdlineLen + 1, sizeof(char));
					numTries++;
				}

				if (pidCommandline)
				{
					// Construct /proc/<PID>/cmdline
					// Copy in /proc/<PID>/
					temp_ptr = strcpy(pidCommandline, retVal->pidName);

					if (temp_ptr != pidCommandline)
					{
						HARKLE_ERROR(Harkleproc, populate_PID_struct, strcpy failed);
						success = false;
					}
					// Concatenate "cmdline" to the end
					temp_ptr = strcat(pidCommandline, "cmdline");

					if (temp_ptr != pidCommandline)
					{
						HARKLE_ERROR(Harkleproc, populate_PID_struct, strcat failed);
						success = false;
					}
					cmdlineLen = strlen(pidCommandline);  // ...for safety
					// fprintf(stdout, "pidCommandline %s is %zu characters long\n", pidCommandline, cmdlineLen);  // DEBUGGING
					// fprintf(stdout, "%s is %zu characters long\n", "/proc/1/cmdline", strlen("/proc/1/cmdline"));  // DEBUGGING

					if (cmdlineLen >= strlen("/proc/1/cmdline") && success == true)  // Minimum length
					{
						retVal->pidCmdline = read_a_file(pidCommandline);

						if (retVal->pidCmdline)
						{
							// 2.4 bool stillExists
							retVal->stillExists = true;
						}
						else
						{
							HARKLE_ERROR(Harkleproc, populate_PID_struct, read_a_file failed);
							// success = false;
							retVal->stillExists = false;
						}
					}
					else
					{
						HARKLE_ERROR(Harkleproc, populate_PID_struct, /proc/<PID>/cmdline array too short);
						success = false;
					}
				}
				else
				{
					HARKLE_ERROR(Harkleproc, populate_PID_struct, /proc/<PID>/cmdline array allocation failed);
					success = false;
				}
			}
			else
			{
				HARKLE_ERROR(Harkleproc, populate_PID_struct, copy_a_string failed);
				success = false;
			}
		}
		else
		{
			HARKLE_ERROR(Harkleproc, populate_PID_struct, create_PID_struct failed);
			success = false;
		}
	}

	// CLEAN UP?
	// If anything failed, clean up everything
	if (success == false)
	{
		if (false == free_PID_struct(&retVal))
		{
			// fprintf(stderr, "<<<ERROR>>> - Harkleproc - populate_PID_struct() - free_PID_struct failed!\n");
			HARKLE_ERROR(Harkleproc, populate_PID_struct, free_PID_struct failed);
		}
	}

	// Clean up this temporary char array regardless of success or failure
	if (pidCommandline)
	{
		if (false == release_a_string(&pidCommandline))
		{
			HARKLE_ERROR(Harkleproc, populate_PID_struct, release_a_string failed);
		}
	}

	// DONE
	return retVal;
}


bool free_PID_struct(pidDetails_ptr* pidDetailsStruct_ptr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	pidDetails_ptr tmpStruct_ptr = NULL;  // Easier to deal with this way

	// INPUT VALIDATION
	if (pidDetailsStruct_ptr)
	{
		if (*pidDetailsStruct_ptr)
		{
			tmpStruct_ptr = *pidDetailsStruct_ptr;

			// 1. pid_t pidNum;			// PID number
			tmpStruct_ptr->pidNum = 0;

			// 2. char* pidName;		  // Absolute path of PID
			if (tmpStruct_ptr->pidName)
			{
				// fprintf(stdout, "BEFORE release_a_string(&pidName), (*currStruct_arr)->pidName == %p\n", tmpStruct_ptr->pidName);  // DEBUGGING
				if (false == release_a_string(&(tmpStruct_ptr->pidName)))
				{
					HARKLE_ERROR(Harkleproc, free_PID_struct, release_a_string failed);
					retVal = false;
				}
				// fprintf(stdout, "AFTER  release_a_string(&pidName), (*currStruct_arr)->pidName == %p\n", tmpStruct_ptr->pidName);  // DEBUGGING
			}

			// 3. char* pidCmdline;	   // Complete cmdline used to execute the PID
			if (tmpStruct_ptr->pidCmdline)
			{
				if (false == release_a_string(&(tmpStruct_ptr->pidCmdline)))
				{
					HARKLE_ERROR(Harkleproc, free_PID_struct, release_a_string failed);
					retVal = false;
				}
			}

			// 4. bool stillExists;	   // False if PID ever disappears
			// Just in case someone would think about accessing this
			tmpStruct_ptr->stillExists = false;

			// 5. Free pidDetailsStruct_ptr
			free(*pidDetailsStruct_ptr);
			
			// 6. NULL pidDetailsStruct_ptr
			*pidDetailsStruct_ptr = NULL;
		}
		else
		{
			HARKLE_ERROR(Harkleproc, free_PID_struct, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, free_PID_struct, NULL pointer);
		retVal = false;
	}

	// DONE
	return retVal;
}


bool free_PID_struct_arr(pidDetails_ptr** pidDetails_arr)
{
	// LOCAL VARIABLES
	bool retVal = true;
	bool freeReturn = true;  // Indicates success or failure of free_PID_struct()
	pidDetails_ptr* currStruct_arr = NULL;  // Will hold the pointer to the array

	// INPUT VALIDATION
	if (pidDetails_arr)
	{
		if (*pidDetails_arr)
		{
			currStruct_arr = *pidDetails_arr;

			// 1. Free each struct pointer
			while (*currStruct_arr && freeReturn == true)
			{
				// currStruct_ptr = *currStruct_arr;

				// fprintf(stdout, "BEFORE currStruct_arr:\t%p\n", currStruct_arr);  // DEBUGGING
				// fprintf(stdout, "BEFORE *currStruct_arr:\t%p\n", *currStruct_arr);  // DEBUGGING
				// fprintf(stdout, "BEFORE free_PID_struct(currStruct_arr), (*currStruct_arr)->pidName == %p\n", (*currStruct_arr)->pidName);  // DEBUGGING
				freeReturn = free_PID_struct(currStruct_arr);
				// fprintf(stdout, "AFTER currStruct_arr:\t%p\n", currStruct_arr);  // DEBUGGING
				// fprintf(stdout, "AFTER *currStruct_arr:\t%p\n", *currStruct_arr);  // DEBUGGING


				if (freeReturn == false)
				{
					HARKLE_ERROR(Harkleproc, free_PID_struct_arr, free_PID_struct failed);
					retVal = false;
				}
				else
				{
					// Next struct pointer
					currStruct_arr++;
				}
			}

			// fprintf(stdout, "BEFORE free()\tpidDetails_arr == %p\t*pidDetails_arr == %p\n", pidDetails_arr, *pidDetails_arr);  // DEBUGGING
			// 2. Free the struct array
			free(*pidDetails_arr);
			// fprintf(stdout, "AFTER  free()\tpidDetails_arr == %p\t*pidDetails_arr == %p\n", pidDetails_arr, *pidDetails_arr);  // DEBUGGING

			// 3. NULL the struct array
			*pidDetails_arr = NULL;
		}
		else
		{
			HARKLE_ERROR(Harkleproc, free_PID_struct_arr, NULL pointer);
			retVal = false;
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, free_PID_struct_arr, NULL pointer);
		retVal = false;
	}

	// DONE
	return retVal;
}


char** parse_proc_PIDs(void)
{
	// LOCAL VARIABLES
	char** retVal = NULL;
	dirDetails_ptr procDetails_ptr = NULL;

	// 1. walk_proc()
	// puts("1. walk_proc()");  // DEBUGGING
	procDetails_ptr = walk_proc();

	// 2. parse_PID_dirs_to_arr()
	// puts("2. parse_PID_dirs_to_arr()");  // DEBUGGING
	if (procDetails_ptr)
	{
		retVal = parse_PID_dirs_to_arr(procDetails_ptr);

		if (retVal == false)
		{
			HARKLE_ERROR(Harkleproc, parse_proc_PIDs, parse_PID_dirs_to_arr failed);
			retVal = false;
		}
	}

	// 3. free_dirDetails_ptr()
	// puts("3. free_dirDetails_ptr()");  // DEBUGGING
	if (procDetails_ptr)
	{
		if (false == free_dirDetails_ptr(&procDetails_ptr))
		{
			HARKLE_ERROR(Harkleproc, parse_proc_PIDs, free_dirDetails_ptr failed);
			retVal = false;
		}
	}

	// DONE
	return retVal;
}


pidDetails_ptr* parse_proc_PID_structs(void)
{
	// LOCAL VARIABLES
	pidDetails_ptr* retVal = NULL;
	dirDetails_ptr procDetails_ptr = NULL;
	bool success = true;  // Change this to false if anything fails

	// 1. walk_proc()
	// puts("1. parse_proc_PID_structs() calls walk_proc()");  // DEBUGGING
	if (success == true)
	{
		procDetails_ptr = walk_proc();

		if (!procDetails_ptr)
		{
			HARKLE_ERROR(Harkleproc, parse_proc_PID_structs, walk_proc failed);
			success = false;
		}
	}

	// 2. parse_PID_dirs_to_struct_arr()
	// puts("2. parse_proc_PID_structs() calls parse_PID_dirs_to_struct_arr()");  // DEBUGGING
	if (success == true && procDetails_ptr)
	{
		retVal = parse_PID_dirs_to_struct_arr(procDetails_ptr);

		if (!retVal)
		{
			HARKLE_ERROR(Harkleproc, parse_proc_PID_structs, parse_PID_dirs_to_struct_arr failed);
			success = false;
		}
	}

	// 3. free_dirDetails_ptr()
	// puts("3. parse_proc_PID_structs() calls free_dirDetails_ptr()");  // DEBUGGING
	if (procDetails_ptr)
	{
		if (false == free_dirDetails_ptr(&procDetails_ptr))
		{
			HARKLE_ERROR(Harkleproc, parse_proc_PID_structs, free_dirDetails_ptr failed);
			success = false;
		}
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_PID_struct_arr(&retVal))
			{
				HARKLE_ERROR(Harkleproc, parse_proc_PID_structs, free_PID_struct_arr failed);
			}
		}
	}

	// DONE
	return retVal;
}


dirDetails_ptr walk_proc(void)
{
	// LOCAL VARIABLES
	dirDetails_ptr retVal = open_dir("/proc");

	if (!retVal)
	{
		HARKLE_ERROR(Harkleproc, walk_proc, open_dir failed);
	}

	// DONE
	return retVal;
}


char** parse_PID_dirs_to_arr(dirDetails_ptr procWalk_ptr)
{
	// LOCAL VARIABLES
	char** retVal = NULL;  // Return value
	char** temp_ptr = NULL;  // Iterating variable
	hdEnt_ptr* tempFN_ptr = NULL;  // Iterating variable for the dirName_arr
	int numTries = 0;  // Tracks attempts to avoid upper end limit of allocation attempts
	int numNames = 0;  // Number of names to add
	int i = 0;  // Loop iterating variable
	bool success = true;  // Make this false if anything fails

	// INPUT VALIDATION
	if (procWalk_ptr)
	{
		// Allocate the array of char pointers
		if (procWalk_ptr->numDirs > 0 && procWalk_ptr->dirName_arr)
		{
			numNames = procWalk_ptr->numDirs;
			tempFN_ptr = procWalk_ptr->dirName_arr;
			while (retVal == NULL && numTries < HPROC_MAX_TRIES)
			{
				retVal = (char**)calloc(numNames + 1, sizeof(char*));
				numTries++;
			}

			if (retVal)
			{
				temp_ptr = retVal;
				
				for (i = 0; i < numNames; i++)
				{
					if (true == is_it_a_PID((*tempFN_ptr)->hd_Name))
					{
						// fprintf(stdout, "%s is a PID!\n", *tempFN_ptr);  // DEBUGGING
						(*temp_ptr) = copy_a_string((*tempFN_ptr)->hd_Name);

						if(!(*temp_ptr))
						{
							HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_arr, copy_a_string failed);
							success = false;
						}
						else
						{
							temp_ptr++;
						}
					}
					tempFN_ptr++;
				}
			}
			else
			{
				HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_arr, calloc failed);
				success = false;
			}
		}
		else
		{
			HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_arr, Invalid dirDetails struct);
			success = false;
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_arr, NULL pointer);
		success = false;
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_char_arr(&retVal))
			{
				HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_arr, free_char_arr failed);
			}
		}
	}

	// DONE
	return retVal;
}


pidDetails_ptr* parse_PID_dirs_to_struct_arr(dirDetails_ptr procWalk_ptr)
{
	// LOCAL VARIABLES
	pidDetails_ptr* retVal = NULL;  // Return value
	pidDetails_ptr* temp_ptr = NULL;  // Iterating variable
	hdEnt_ptr* tempFN_ptr = NULL;  // Iterating variable for the dirName_arr
	int numTries = 0;  // Tracks attempts to avoid upper end limit of allocation attempts
	int numNames = 0;  // Number of names to add
	int i = 0;  // Loop iterating variable
	char templateProc[32] = { "/proc/4194303/" };  // Array to utilize as /proc/<PID>/ absolute template
	int templateSize = sizeof(templateProc) / sizeof(*templateProc);
	bool success = true;  // If anything fails, make this false

	// INPUT VALIDATION
	if (procWalk_ptr)
	{
		// Allocate the array of char pointers
		if (procWalk_ptr->numDirs > 0 && procWalk_ptr->dirName_arr)
		{
			numNames = procWalk_ptr->numDirs;
			tempFN_ptr = procWalk_ptr->dirName_arr;
			while (retVal == NULL && numTries < HPROC_MAX_TRIES)
			{
				retVal = (pidDetails_ptr*)calloc(numNames + 1, sizeof(pidDetails_ptr));
				numTries++;
			}

			if (retVal)
			{
				temp_ptr = retVal;
				
				for (i = 0; i < numNames; i++)
				{
					if (true == is_it_a_PID((*tempFN_ptr)->hd_Name))
					{
						// Create an absolute path to the PID dir
						if (true == proc_builder(templateProc, (*tempFN_ptr)->hd_Name, templateSize))
						{
							*temp_ptr = populate_PID_struct(templateProc);

							if(!(*temp_ptr))
							{
								HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, populate_PID_struct failed);
								success = false;
							}
							else
							{
								temp_ptr++;
							}
						}
						else
						{
							HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, proc_builder failed);
							success = false;
						}
					}
					tempFN_ptr++;
				}
			}
			else
			{
				HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, calloc failed);
				success = false;
			}
		}
		else
		{
			HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, invalid dirDetails struct);
			success = false;
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, NULL pointer);
		success = false;
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == free_PID_struct_arr(&retVal))
			{
				HARKLE_ERROR(Harkleproc, parse_PID_dirs_to_struct_arr, free_PID_struct_arr failed);
			}
		}
	}

	// DONE
	return retVal;
}


bool is_it_a_PID(char* dirName)
{
	// LOCAL VARIABLES
	bool retVal = true;  // Default answer to prove incorrect
	char* temp_ptr = NULL;  // Iterating variable

	// INPUT VALIDATION
	if (dirName)
	{
		if (*dirName)
		{
			temp_ptr = dirName;

			while (*temp_ptr && retVal == true)
			{
				if (*temp_ptr < 48 || *temp_ptr > 57)
				{
					retVal = false;
				}
				else
				{
					temp_ptr++;
				}
			}
		}
		else
		{
			retVal = false;
		}
	}
	else
	{
		retVal = false;
	}

	// DONE
	return retVal;
}


char* make_PID_into_proc(char* pidNum)
{
	// LOCAL VARIABLES
	char* retVal = NULL;
	bool success = true;  // If anything fails, make this false
	size_t pidLen = 0;  // Get the length of the PID
	size_t procPIDLen = 0;  // Length of return value

	// INPUT VALIDATION
	// 1. NULL Pointer
	if (!pidNum)
	{
		HARKLE_ERROR(Harkleproc, make_PID_into_proc, NULL pointer);
		success = false;
	}
	else
	{
		pidLen = strlen(pidNum);

		// 2. Empty string
		if (pidLen < 1)
		{
			HARKLE_ERROR(Harkleproc, make_PID_into_proc, empty string);
			success = false;
		}
		else
		{
			// 3. Is it a PID
			if (false == is_it_a_PID(pidNum))
			{
				HARKLE_ERROR(Harkleproc, make_PID_into_proc, Not a PID);
				success = false;
			}
		}
	}

	// BUILD IT
	if (success == true)
	{
		// 1. Determine size
		procPIDLen = pidLen + sizeof("/proc//");

		// 2. Allocate a buffer
		retVal = get_me_a_buffer(procPIDLen);

		if (!retVal)
		{
			HARKLE_ERROR(Harkleproc, make_PID_into_proc, get_me_a_buffer failed);
			success = false;
		}
		// 3. Build the absolute /proc/<PID>/ path
		else
		{
			if (false == proc_builder(retVal, pidNum, procPIDLen + 1))
			{
				HARKLE_ERROR(Harkleproc, make_PID_into_proc, proc_builder failed);
				success = false;
			}
		}
	}

	// CLEAN UP
	if (success == false)
	{
		if (retVal)
		{
			if (false == release_a_string_len(&retVal, procPIDLen))
			{
				HARKLE_ERROR(Harkleproc, make_PID_into_proc, release_a_string_len failed);
			}
		}
	}

	// DONE
	return retVal;
}


//////////////////////////////////////////////////////////////////////////////
///////////////////////// LOCAL FUNCTION DEFINITIONS /////////////////////////
//////////////////////////////////////////////////////////////////////////////


bool proc_builder(char* buf, char* PID, size_t bufSize)
{
	// LOCAL VARIABLES
	bool retVal = true;
	char* temp_ptr = NULL;  // Holds return values from string.h function calls
	char* tempBuf_ptr = buf;  // Iterating variable starting at buf
	char procPath[] = { "/proc/"};

	// INPUT VALIDATION
	if (!buf)
	{
		HARKLE_ERROR(Harkleproc, proc_builder, NULL pointer);
		retVal = false;
	}
	else if (!PID)
	{
		HARKLE_ERROR(Harkleproc, proc_builder, NULL pointer);
		retVal = false;
	}
	else if (!(*PID))
	{
		HARKLE_ERROR(Harkleproc, proc_builder, empty PID);
		retVal = false;
	}
	else if (bufSize < 0)
	{
		HARKLE_ERROR(Harkleproc, proc_builder, Invalid buf size);
		retVal = false;
	}
	else if (bufSize < (strlen(PID) + strlen("/proc//" + 1)))
	{
		HARKLE_ERROR(Harkleproc, proc_builder, buf too small);
		retVal = false;
	}

	// BUILD /proc/<PID>/
	// 1. memset buf
	temp_ptr = memset(buf, 0x0, bufSize);

	if (temp_ptr == buf)
	{
		// 2. Precursor
		temp_ptr = procPath;

		while (*temp_ptr)
		{
			*tempBuf_ptr = *temp_ptr;
			tempBuf_ptr++;
			temp_ptr++;
		}

		// 3. PID
		temp_ptr = PID;

		while (*temp_ptr)
		{
			*tempBuf_ptr = *temp_ptr;
			tempBuf_ptr++;
			temp_ptr++;
		}

		// 4. Trailing slash
		tempBuf_ptr--;  // Walk it back one to check for a trailing slash
		if (*tempBuf_ptr != '/')
		{
			tempBuf_ptr++;
			*tempBuf_ptr = '/';
		}
	}
	else
	{
		HARKLE_ERROR(Harkleproc, proc_builder, memset failed);
		retVal = false;
	}

	// DONE
	return retVal;
}


/*
	PURPOSE - Translate a PID string into a pid_t value
	INPUT
		PID - Nul-terminated string representing a PID
	OUTPUT
		On success, pid_t value based on PID
		On failure, -1
 */
pid_t convert_PID(char* PID)
{
	// LOCAL VARIABLES
	pid_t retVal = -1;
	int errNum = 0;  // Store errno here on failure
	bool success = true;  // Make this false if anything fails
	char* tmp_ptr = NULL;
	char* endptr;  // Necessary for the strtoimax() call

	// INPUT VALIDATION
	if (!PID)
	{
		HARKLE_ERROR(Harkleproc, convert_PID, NULL pointer);
		success = false;
	}
	else if (!(*PID))
	{
		HARKLE_ERROR(Harkleproc, convert_PID, Empty PID);
		success = false;
	}
	else
	{
		tmp_ptr = PID;

		while (*tmp_ptr)
		{
			if (*tmp_ptr < '0' || *tmp_ptr > '9')
			{
				success = false;
				break;
			}
			else
			{
				tmp_ptr++;
			}
		}
	}

	// CONVERT
	if (true == success)
	{
		retVal = strtoimax(PID, &endptr, 10);

		if (0 == retVal)
		{
			errNum = errno;
			HARKLE_ERROR(Harkleproc, convert_PID, strtoimax failed);
			fprintf(stderr, "strtoimax() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
	}

	// DONE
	// fprintf(stdout, "Converted '%s' to '%jd'\n", PID, (intmax_t)retVal);  // DEBUGGING
	return retVal;
}
