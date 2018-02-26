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
 	int pidNum = 0;  // Tracks PID count
 	char* userPID = NULL;  // Will hold char* holding user's choice of PID
 	char* userProcPID = NULL;  // Will hold char* holding /proc/<PID>/ built from user's choice

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
 		puts("<<<ERROR>>> - main() - parse_proc_PID_structs has failed!");
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
 		}
 		else
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - make_PID_into_proc has failed!");
 		}
 	}
 	else
 	{
 		fprintf(stderr, "<<<ERROR>>> - main() - buff_a_num has failed!");
 	}
 	
 	// CLEAN UP
 	// 1. procPIDStructs
 	if (procPIDStructs)
 	{
 		if (false == free_PID_struct_arr(&procPIDStructs))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - free_PID_struct_arr has failed!");
 		}
 	}
 	temp_arr = NULL;

 	// 2. userPID
 	if (userPID)
 	{
 		if (false == release_a_string(&userPID))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!");
 		}
 	}

 	// 3. userProcPID
 	if (userProcPID)
 	{
 		if (false == release_a_string(&userProcPID))
 		{
 			fprintf(stderr, "<<<ERROR>>> - main() - release_a_string has failed!");
 		}
 	}

 	// DONE
	return 0;
 }
 
