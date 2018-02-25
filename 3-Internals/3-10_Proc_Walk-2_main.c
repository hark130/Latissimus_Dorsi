#include "Harkledir.h"
#include "Harkleproc.h"
#include <stdio.h>

 
 int main(void)
 {
 	// LOCAL VARIABLES
 	pidDetails_ptr* procPIDStructs = parse_proc_PID_structs();
 	pidDetails_ptr* temp_arr = procPIDStructs;
 	pidDetails_ptr tempStruct_ptr = NULL;  // Single struct from the array
 	int pidNum = 0;  // Tracks PID count

 	// INPUT VALIDATION
 	if (procPIDStructs)
 	{
 		while (temp_arr)
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
 	}
 	
 	// CLEAN UP
 	if (procPIDStructs)
 	{
 		if (false == free_PID_struct_arr(&procPIDStructs))
 		{
 			puts("<<<ERROR>>> - main() - free_PID_struct_arr has failed!");
 		}
 	}
 	temp_arr = NULL;

 	// DONE
	return 0;
 }
 
