/*
	This source code's entire purprose is to:
		Take a PID as an argument
		Print the libraries opened by that PID
	This source is the culmination of all the work completed on 3-10-2.
 */

#include "Fileroad.h"
#include "Harkledir.h"
#include "Harkleproc.h"
#include "Memoroad.h"
#include <stdio.h>
#include <stdbool.h>	// bool, true, false
#include <string.h>

 
 int main(int argc, char* argv[])
 {
 	// LOCAL VARIABLES
 	bool success = true;  // If anything fails, set this to false
 	pidDetails_ptr* procPIDStructs = parse_proc_PID_structs();
 	pidDetails_ptr* temp_arr = procPIDStructs;
 	pidDetails_ptr tempStruct_ptr = NULL;  // Single struct from the array
 	dirDetails_ptr userPIDMapFiles = NULL;  // Return value from open_dir()
 	hdEnt_ptr* hdEnt_arr = NULL;
 	hdEnt_ptr hdEntStruct_ptr = NULL;
 	int pidNum = 0;  // Tracks PID count
 	// char* userPID = NULL;  // Will hold char* holding user's choice of PID
 	char* userProcPID = NULL;  // Will hold char* holding /proc/<PID>/ built from user's choice
 	char* userProcPIDMapFiles = NULL;  // Will hold char* with user's /proc/<PID>/map_files/ choice
 	// char* userProcPIDMaps = NULL;  // Will hold char* with user's /proc/<PID>/maps choice
 	// char* pidMapsContents = NULL;  // Contents of user's /proc/<PID>/maps

 	// INPUT VALIDATION
 	// procPIDStructs
 	if (!procPIDStructs)
 	{
 		puts("<<<ERROR>>> - print_PID_libraries - parse_proc_PID_structs has failed!\n\n");
 		success = false;
 	}

 	if (argc < 2)
 	{
 		fprintf(stderr, "\nToo few arguments!\nusage: print_PID_libraries.exe <PID>\n\n");
 		success = false;
 	}
 	else if (argc > 2)
 	{
 		fprintf(stderr, "\nToo many arguments!\nusage: print_PID_libraries.exe <PID>\n\n");
 		success = false;
 	}
 	else if (false == is_it_a_PID(argv[1]))
 	{
 		fprintf(stderr, "\nInvalid PID!\nusage: print_PID_libraries.exe <PID>\nexample: print_PID_libraries.exe 1234\n\n");
 		success = false;
 	}
 	else
 	{
 		// Create absolute path of PID
 		userProcPID = make_PID_into_proc(argv[1]);

 		if (!userProcPID)
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - print_PID_libraries - make_PID_into_proc has failed!\n\n");
 			success = false;
 		}
 		else
 		{
	 		success = false;  // Set to true when PID match is found
	 		while (*temp_arr && success == false)
	 		{
	 			// fprintf(stdout, "userProcPID: %s\n", userProcPID);  // DEBUGGING
	 			// fprintf(stdout, "pdName:      %s\n", (*temp_arr)->pidName);  // DEBUGGING
	 			// fprintf(stdout, "strcmp(%s, %s) == %d\n\n", (*temp_arr)->pidName, userProcPID, strcmp((*temp_arr)->pidName, userProcPID));  // DEBUGGING
	 			if (0 == strcmp((*temp_arr)->pidName, userProcPID) && (*temp_arr)->stillExists == true)
	 			{
	 				success = true;
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
 			// fprintf(stdout, "\nProcessing directory %s\n\n", userProcPIDMapFiles);  // DEBUGGING
 			userPIDMapFiles = open_dir(userProcPIDMapFiles);

 			if (!userPIDMapFiles)
 			{
	 			fprintf(stderr, "\n<<<ERROR>>> - main() - open_dir has failed!\n");
	 			success = false;
 			}
 		}
 		else
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - main() - os_path_join has failed!\n");
 			success = false;
 		}
 	}
 	
 	// Print directory contents of /proc/<PID>/map_files/
 	if (success == true && userPIDMapFiles)
 	{
 		fprintf(stdout, "\nLibraries loaded by %s:\n\n", userProcPID);
 		hdEnt_arr = userPIDMapFiles->fileName_arr;

 		if (hdEnt_arr)
 		{
 			while (*hdEnt_arr)
 			{
 				hdEntStruct_ptr = *hdEnt_arr;

 				if (hdEntStruct_ptr)
 				{
 					if (hdEntStruct_ptr->hd_type == DT_LNK)  // Symbolic link d_type
 					{
	 					if (hdEntStruct_ptr->hd_symName)  // Resolved symbolic link name
	 					{
	 						fprintf(stdout, "\t%s\n", hdEntStruct_ptr->hd_symName);
	 					}
 					}
 				}

 				// Next struct
 				hdEnt_arr++;
 			}
 		}
 	}

 	// CLEAN UP
 	// 1. procPIDStructs
 	if (procPIDStructs)
 	{
 		if (false == free_PID_struct_arr(&procPIDStructs))
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - main() - free_PID_struct_arr has failed!\n\n");
 		}
 	}
 	temp_arr = NULL;

 	// // 2. userPID
 	// if (userPID)
 	// {
 	// 	if (false == release_a_string(&userPID))
 	// 	{
 	// 		fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 	// 	}
 	// }

 	// 3. userProcPID
 	if (userProcPID)
 	{
 		if (false == release_a_string(&userProcPID))
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - main() - release_a_string has failed!\n\n");
 		}
 	}

 	// 4. userProcPIDMapFiles
 	if (userProcPIDMapFiles)
 	{
 		if (false == release_a_string(&userProcPIDMapFiles))
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - main() - release_a_string has failed!\n\n");
 		} 			
 	}

 	// 5. userPIDMapFiles
 	if (userPIDMapFiles)
 	{
 		if (false == free_dirDetails_ptr(&userPIDMapFiles))
 		{
 			fprintf(stderr, "\n<<<ERROR>>> - main() - free_dirDetails_ptr has failed!\n\n");
 		}
 	}

 	// // 6. userProcPIDMaps
 	// if (userProcPIDMaps)
 	// {
 	// 	if (false == release_a_string(&userProcPIDMaps))
 	// 	{
 	// 		fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 	// 	}
 	// }

 	// // 7. pidMapsContents
 	// if (pidMapsContents)
 	// {
 	// 	if (false == release_a_string(&pidMapsContents))
 	// 	{
 	// 		fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 	// 	}
 	// }

 	// DONE
	return 0;
 }
 
