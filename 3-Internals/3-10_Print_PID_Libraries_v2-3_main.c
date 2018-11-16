/*
	This source code's entire purprose is to:
		Take a PID as an argument
		Print the libraries opened by that PID
	This source is the culmination of all the work completed on 3-10-2.
 */

#include "Fileroad.h"
#include "Harkledir.h"
#include "Harkleproc.h"
#include "Harklerror.h"	// HARKLE_ERROR
#include "Memoroad.h"
#include <stdio.h>
#include <stdbool.h>	// bool, true, false
#include <string.h>


void print_usage(void);

 
 int main(int argc, char* argv[])
 {
	// LOCAL VARIABLES
	bool success = true;  // If anything fails, set this to false
	pidDetails_ptr *procPIDStructs = parse_proc_PID_structs();
	pidDetails_ptr *temp_arr = procPIDStructs;
	pidDetails_ptr tempStruct_ptr = NULL;  // Single struct from the array
	dirDetails_ptr userPIDMapFiles = NULL;  // Return value from open_dir()
	int pidNum = 0;  // Tracks PID count
	char *userProcPID = NULL;  // Will hold char* holding /proc/<PID>/ built from user's choice
	char *userProcPIDMapFiles = NULL;  // Will hold char* with user's /proc/<PID>/map_files/ choice
	char **uniqueSymNames = NULL;  // Return value from parse_dirDetails_to_char_arr()
	char **tempChar_arr = NULL;  // Iterating variable for uniqueSymNames
	char *newVerArgs[] = { "-v2", "--version2" };
	bool newVer = false;  // Set this to true if newVerArgs makes a match with argv[2]
	int i = 0;  // Iterating variable
	char *pid_ptr = NULL;  // Store the appropriate PID pointer here

	// INPUT VALIDATION
	// procPIDStructs
	if (!procPIDStructs)
	{
		HARKLE_ERROR(print_PID_libraries, main, parse_proc_PID_structs has failed);
		// puts("<<<ERROR>>> - print_PID_libraries - parse_proc_PID_structs has failed!\n\n");
		success = false;
	}

	if (argc < 2)
	{
		fprintf(stderr, "\nToo few arguments!\n");
		print_usage();
		success = false;
	}
	else if (2 == argc)
	{
		newVer = false;
		pid_ptr = argv[1];
	}
	else if (3 == argc)
	{
		success = false;  // Check it after looping

		for (i = 0; i < (sizeof(newVerArgs)/sizeof(*newVerArgs)); i++)
		{
			if (!strcmp(argv[1], (*(newVerArgs + i))))
			{
				success = true;  // Found a match
				newVer = true;  // User wants the new version
				break;  // Stop looking
			}
		}

		if (false == success)
		{
			fprintf(stderr, "\nToo few arguments!\n");
			print_usage();
			success = false;
		}
		else
		{
			pid_ptr = argv[2];
		}
	}
	else
	{
		fprintf(stderr, "\nToo many arguments!\n");
		print_usage();
		success = false;
	}

	// Check the PID
	if (true == success && false == newVer && false == is_it_a_PID(pid_ptr))
	{
		fprintf(stderr, "\nInvalid PID!\n");
		fprintf(stderr, "example: print_PID_libraries.exe 1234\n");
		print_usage();
		success = false;
	}
	else if (true == success && true == newVer && false == is_it_a_PID(pid_ptr))
	{
		fprintf(stderr, "\nInvalid PID!\n");
		fprintf(stderr, "example: print_PID_libraries.exe 1234\n");
		print_usage();
		success = false;
	}
	else if (true == success)
	{
		// Create absolute path of PID
		userProcPID = make_PID_into_proc(pid_ptr);

		if (!userProcPID)
		{
			HARKLE_ERROR(print_PID_libraries, main, make_PID_into_proc has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - make_PID_into_proc has failed!\n\n");
			success = false;
		}
		else
		{
			success = false;  // Set to true when PID match is found
			while (*temp_arr && success == false)
			{
				if (0 == strcmp((*temp_arr)->pidName, userProcPID) && (*temp_arr)->stillExists == true)
				{
					success = true;
					fprintf(stdout, "\nParsing %s (%s)\n", (*temp_arr)->pidName, (*temp_arr)->pidCmdline);
				}
				else
				{
					temp_arr++;
				}
			}

			if (success == false)
			{
				fprintf(stderr, "\nUnable to locate %s.\n\n", userProcPID);
			}
		}
	}

	// PARSE /proc/<userPID>/map_files
	if (success == true)
	{
		userProcPIDMapFiles = os_path_join(userProcPID, "/map_files/", false);

		if (userProcPIDMapFiles)
		{
			userPIDMapFiles = open_dir(userProcPIDMapFiles);

			if (!userPIDMapFiles)
			{
				HARKLE_ERROR(print_PID_libraries, main, open_dir has failed);
				// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - open_dir has failed!\n");
				success = false;
			}
		}
		else
		{
			HARKLE_ERROR(print_PID_libraries, main, os_path_join has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - os_path_join has failed!\n");
			success = false;
		}
	}
	
	// Print directory contents of /proc/<PID>/map_files/
	if (success == true && userPIDMapFiles)
	{
		if (userPIDMapFiles->numFiles > 0)
		{
			uniqueSymNames = parse_dirDetails_to_char_arr(userPIDMapFiles, HDIR_DT_LNK, true, true);

			if (!uniqueSymNames)
			{
				HARKLE_ERROR(print_PID_libraries, main, parse_dirDetails_to_char_arr has failed with a NULL pointer);
				// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - parse_dirDetails_to_char_arr has failed with a NULL pointer!\n");
				success = false;
			}
			else if (!(*uniqueSymNames))
			{
				HARKLE_ERROR(print_PID_libraries, main, parse_dirDetails_to_char_arr has failed with an empty array);
				// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - parse_dirDetails_to_char_arr has failed with an empty array!\n");
				success = false;
			}
		}
	}

	// Print the unique entries
	if (success == true && uniqueSymNames)
	{
		fprintf(stdout, "Files loaded by %s:\n\n", userProcPID);

		tempChar_arr = uniqueSymNames;

		while (*tempChar_arr)
		{
			fprintf(stdout, "\t%s\n", *tempChar_arr);
			tempChar_arr++;
		}
		fprintf(stdout, "\n\n");
	}
	else if (success == true)
	{
		fprintf(stdout, "\n%s does not have any files loaded.\n\n", userProcPID);
	}

	// CLEAN UP
	// 1. procPIDStructs
	if (procPIDStructs)
	{
		if (false == free_PID_struct_arr(&procPIDStructs))
		{
			HARKLE_ERROR(print_PID_libraries, main, free_PID_struct_arr has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - free_PID_struct_arr has failed!\n\n");
		}
	}
	temp_arr = NULL;

	// 2. userProcPID
	if (userProcPID)
	{
		if (false == release_a_string(&userProcPID))
		{
			HARKLE_ERROR(print_PID_libraries, main, release_a_string has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - release_a_string has failed!\n\n");
		}
	}

	// 3. userProcPIDMapFiles
	if (userProcPIDMapFiles)
	{
		if (false == release_a_string(&userProcPIDMapFiles))
		{
			HARKLE_ERROR(print_PID_libraries, main, release_a_string has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - release_a_string has failed!\n\n");
		}			
	}

	// 4. userPIDMapFiles
	if (userPIDMapFiles)
	{
		if (false == free_dirDetails_ptr(&userPIDMapFiles))
		{
			HARKLE_ERROR(print_PID_libraries, main, free_dirDetails_ptr has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - free_dirDetails_ptr has failed!\n\n");
		}
	}

	// 5. uniqueSymNames
	if (uniqueSymNames)
	{
		if (false == free_char_arr(&uniqueSymNames))
		{
			HARKLE_ERROR(print_PID_libraries, main, free_char_arr has failed);
			// fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - free_char_arr has failed!\n\n");
		}
	}

	fprintf(stdout, "\n");

	// DONE
	return 0;
 }
 

void print_usage(void)
{
	fprintf(stderr, "usage: print_PID_libraries.exe <PID>\n");
	fprintf(stderr, "usage: print_PID_libraries.exe -v2 <PID>\n");
	fprintf(stderr, "usage: print_PID_libraries.exe --version2 <PID>\n\n");

	return;
}

