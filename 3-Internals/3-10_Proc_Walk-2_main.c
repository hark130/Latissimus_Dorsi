#include "Fileroad.h"
#include "Harkledir.h"
#include "Harkleproc.h"
#include "Memoroad.h"
#include <stdio.h>
#include <stdbool.h>	// bool, true, false

 
 int main(void)
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
 	char* userPID = NULL;  // Will hold char* holding user's choice of PID
 	char* userProcPID = NULL;  // Will hold char* holding /proc/<PID>/ built from user's choice
 	char* userProcPIDMapFiles = NULL;  // Will hold char* with user's /proc/<PID>/map_files/ choice

 	// INPUT VALIDATION
 	if (procPIDStructs)
 	{
 		while (*temp_arr && success == true)
 		{
 			// Get a struct pointer
 			tempStruct_ptr = *temp_arr;

 			if (tempStruct_ptr)
 			{
	 			// PRINT PROC PID DETAILS
	 			if (tempStruct_ptr->stillExists == true)
	 			{
	 				fprintf(stdout, "PID #%03d:\t%s - %s\n", ++pidNum, tempStruct_ptr->pidName, tempStruct_ptr->pidCmdline);
	 			}
	 			else
	 			{
	 				fprintf(stdout, "PID #%03d:\t%s - %s\n", ++pidNum, tempStruct_ptr->pidName, "No longer exists");
	 			}
	 		}
	 		else
	 		{
	 			fprintf(stdout, "PID #%03d:\t%s - %s\n", ++pidNum, "NULL", "Empty pidDetails struct");
	 		}

 			// Next pidDetails_ptr
 			temp_arr++;
 		}
 	}
 	else
 	{
 		puts("<<<ERROR>>> - main() - parse_proc_PID_structs has failed!\n");
 		success = false;
 	}

 	// PROMPT USER FOR PID
 	fprintf(stdout, "Which PID would you like to investigate?\n");
 	userPID = buff_a_num();
 	if (userPID)
 	{
 		fprintf(stdout, "The user chose:\t%s\n", userPID);  // DEBUGGING

 		userProcPID = make_PID_into_proc(userPID);

 		if (userProcPID)
 		{
 			fprintf(stdout, "Absolute path:\t%s\n", userProcPID);  // DEBUGGING

 			// Verify the directory actually exists
 			/////////////////////////////////////// IMPLEMENT THIS LATER ///////////////////////////////////////
 		}
 		else
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - make_PID_into_proc has failed!\n");
 			success = false;
 		}
 	}
 	else
 	{
 		fprintf(stderr, "<<<ERROR>>> - main() - buff_a_num has failed!\n");
 		success = false;
 	}

 	// PARSE /proc/<userPID>/map_files
 	if (success == true)
 	{
 		userProcPIDMapFiles = os_path_join(userProcPID, "/map_files/", false);

 		if (userProcPIDMapFiles)
 		{
 			fprintf(stdout, "Processing directory %s\n", userProcPIDMapFiles);  // DEBUGGING
 			userPIDMapFiles = open_dir(userProcPIDMapFiles);

 			if (!userPIDMapFiles)
 			{
	 			fprintf(stderr, "<<<ERROR>>> - main() - open_dir has failed!\n");
	 			success = false;
 			}
 		}
 		else
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - os_path_join has failed!\n");
 			success = false;
 		}
 	}
 	
 	// Print directory contents of /proc/<PID>/map_files/
 	if (success == true && userPIDMapFiles)
 	{
 		fprintf(stdout, "FILES FOUND IN %s:\n", userPIDMapFiles->dirName);
 		hdEnt_arr = userPIDMapFiles->fileName_arr;

 		if (hdEnt_arr)
 		{
 			while (*hdEnt_arr)
 			{
 				hdEntStruct_ptr = *hdEnt_arr;

 				if (hdEntStruct_ptr)
 				{

 					fprintf(stdout, "Name:\t%s", hdEntStruct_ptr->hd_Name);
 					if (hdEntStruct_ptr->hd_symName)
 					{
 						fprintf(stdout, " (%s)\n", hdEntStruct_ptr->hd_symName);
 					}
 					else
 					{
 						fprintf(stdout, "\n");
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
 			fprintf(stderr, "<<<ERROR>>> - main() - free_PID_struct_arr has failed!\n");
 		}
 	}
 	temp_arr = NULL;

 	// 2. userPID
 	if (userPID)
 	{
 		if (false == release_a_string(&userPID))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 		}
 	}

 	// 3. userProcPID
 	if (userProcPID)
 	{
 		if (false == release_a_string(&userProcPID))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 		}
 	}

 	// 4. userProcPIDMapFiles
 	if (userProcPIDMapFiles)
 	{
 		if (false == release_a_string(&userProcPIDMapFiles))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!\n");
 		} 			
 	}

 	// 5. userPIDMapFiles
 	if (userPIDMapFiles)
 	{
 		if (false == free_dirDetails_ptr(&userPIDMapFiles))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - free_dirDetails_ptr has failed!\n");
 		}
 	}

 	// DONE
	return 0;
 }
 
