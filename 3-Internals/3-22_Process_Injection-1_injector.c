/*
	This code will serve as the "glue" that brings together all of the
		necessary functionality to inject code into a running process.

	CURRENT PLAN:
		1. Attach to the 'victim' process	
		2. Backup the current state of the processor registers
		3. Parse /proc/PID/maps to get some "r-xp" memory
		4. Backup the memory section
		5. Overwrite executable memory section
		6. Update RIP to point to the injected code
		7. Resume execution
		8. Wait until the injected code finishes running and hits a SIGTRAP
		9. Restore the process back to its original state

	APPROACH:
		1. ptrace(PTRACE_ATTACH, pid, NULL, NULL)
		2. ptrace(PTRACE_GETREGS, pid, NULL, &oldRegs)  // struct user_regs_struct
		3. 
		4. Two different approaches (?)
			4.a. ptrace(PTRACE_PEEKTEXT, pid, addr, NULL)  // Only reads one word of data
			4.b. process_vm_readv()
		5. Two different approaches (?)
			5.a. ptrace(PTRACE_POKETEXT, pid, addr, word)
			5.b. process_vm_writev()
		6. Multi-step process
			6.a. struct user_regs_struct regs;
			6.b. memcpy(&regs, &oldregs, sizeof(struct user_regs_struct));
			6.c. regs.rip = addr_of_injected_code;  // Update RIP to point to our injected code
			6.d. ptrace(PTRACE_SETREGS, pid, NULL, &regs);
		7. waitpid(pid, &status, WUNTRACED)
		8. To tell if control has been paused because of a SIGTRAP ("int 0x03")
			8.a. if (WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) { printf("SIGTRAP received\n"); }
		9. Multi-step process
			9.a. ptrace(PTRACE_SETREGS, pid, NULL, &origregs);  // Recover the original registers
			9.b. ptrace(PTRACE_POKETEXT, pid, addr, word);  // Write the original memory back to the process
			9.c. ptrace(PTRACE_DETACH, pid, NULL, NULL);  // Detach from the process and resume execution

	ADDITIONAL PLANs:
		1. Detect where the original function is
		2. Inject our code
		3. Call our function
		4. Our function operates
		5. Then our function calls their function

	NECESSARY PREPARATION:
		"ptrace() returned errno:	Operation not permitted"
			This is due to kernel hardening in Linux disable this behavior by:
				`echo 0 > /proc/sys/kernel/yama/ptrace_scope`
				modifying /etc/sysctl.d/10-ptrace.conf

 */

#include <errno.h>							// errno
#include "Harklerror.h"						// HARKLE_ERROR
#include "Harkleproc.h"						// is_it_a_PID(), make_PID_into_proc()
#include "Memoroad.h"						// release_a_string()
#include "pmparser.h"						// pmStruct_ptr
#include <stdbool.h>						// bool, true, false
#include <stdio.h>							// fprintf()
#include <string.h>							// strcmp()
#include <sys/ptrace.h>						// ptrace()
#include <sys/user.h>						// struct user_regs_struct

int main(int argc, char* argv[])
{
	// LOCAL VARIABLES
	bool success = true;  // Make this false if anything fails
	long ptRetVal = 0;  // ptrace() return value
	int errNum = 0;  // Store errno here
	pidDetails_ptr* procPIDStructs = parse_proc_PID_structs();
	pidDetails_ptr* temp_arr = procPIDStructs;
	pidDetails_ptr vicPID = NULL;  // Store the struct pointer for argv[1] here
	char* userProcPID = NULL;  // Will hold char* holding /proc/<PID>/ built from user's choice
	struct user_regs_struct oldRegs;  // Store the state of the registers here
	pmStruct_ptr procMaps_ptr = NULL;  // /proc/PID/maps for argv[1]
	pmStruct_ptr tmpPM_ptr = NULL;  // Temporary variable to print out certain mappings
	struct iovec* localBackup = NULL;  // Local backup of the PIDs executable memory map

	// INPUT VALIDATION
	// procPIDStructs
	if (!procPIDStructs)
	{
		HARKLE_ERROR(injector, main, parse_proc_PID_structs failed);
		success = false;
	}
	// Arguments
	if (argc < 2)
	{
		fprintf(stderr, "\nToo few arguments!\nusage: injector.exe <PID>\n\n");
		success = false;
	}
	else if (argc > 2)
	{
		fprintf(stderr, "\nToo many arguments!\nusage: injector.exe <PID>\n\n");
		success = false;
	}
	else if (false == is_it_a_PID(argv[1]))
	{
		fprintf(stderr, "\nInvalid PID!\nusage: injector.exe <PID>\nexample: injector.exe 1234\n\n");
		success = false;
	}
	else
	{
		// Create absolute path of PID
		userProcPID = make_PID_into_proc(argv[1]);

		if (!userProcPID)
		{
			HARKLE_ERROR(injector, main, make_PID_into_proc failed);
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
					vicPID = *temp_arr;
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

	// INJECTOR
	// 1. Attach to the 'victim' process
	if (true == success)
	{
		ptRetVal = ptrace(PTRACE_ATTACH, vicPID->pidNum, NULL, NULL);

		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_ATTACH failed);
			fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
	}

	// 2. Backup the current state of the processor registers
	if (true == success)
	{
		ptRetVal = ptrace(PTRACE_GETREGS, vicPID->pidNum, NULL, &oldRegs);

		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_GETREGS failed);
			fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
	}

	// 3. Parse /proc/PID/maps to get some "r-xp" memory
	if (true == success)
	{
		// 3.1. /proc/PID/maps 
		procMaps_ptr = pmparser_parse(vicPID->pidNum);

		if (!procMaps_ptr)
		{
			HARKLE_ERROR(injector, main, pmparser_parse failed);
			success = false;
		}
		else
		{
			tmpPM_ptr = procMaps_ptr;

			while (tmpPM_ptr)
			{
				if (1 == tmpPM_ptr->is_x)
				{
					// pmparser_print(tmpPM_ptr, 0);  // DEBUGGING
					// fprintf(stdout, "\n");  // DEBUGGING
					break;  // Found one
				}

				tmpPM_ptr = tmpPM_ptr->next;
			}
			
		}
	}

	// 4. Backup the memory section
	if (true == success)
	{
		localBackup = copy_remote_to_local(vicPID->pidNum, tmpPM_ptr->addr_start, (size_t)tmpPM_ptr->length);

		if (!localBackup)
		{
			HARKLE_ERROR(injector, main, copy_remote_to_local failed);
			success = false;
		}
		// else
		// {
		// 	for (size_t i = 0; i < localBackup->iov_len; i++)
		// 	{
		// 		if (0 == i % 100)
		// 		{
		// 			fprintf(stdout, "\n");
		// 		}

		// 		// fprintf(stdout, "0x%08X ", (char)(*((char*)localBackup->iov_base + i)));
		// 		char letter = (char)(*((char*)localBackup->iov_base + i));
		// 		if (letter >= ' ' && letter <= '~')
		// 		{
		// 			fprintf(stdout, "%c", letter);
		// 		}
		// 	}
		// 	fprintf(stdout, "\n");
		// }
	}

	// 5. Overwrite executable memory section
	if (true == success)
	{

	}

	// 6. Update RIP to point to the injected code
	if (true == success)
	{

	}

	// 7. Resume execution
	if (true == success)
	{

	}

	// 8. Wait until the injected code finishes running and hits a SIGTRAP
	if (true == success)
	{

	}

	// 9. Restore the process back to its original state
	if (true == success)
	{

	}

	// CLEAN UP
	// 1. procPIDStructs
	if (procPIDStructs)
	{
		if (false == free_PID_struct_arr(&procPIDStructs))
		{
			HARKLE_ERROR(injector, main, free_PID_struct_arr failed);
		}
	}
	temp_arr = NULL;

	// 2. userProcPID
	if (userProcPID)
	{
		if (false == release_a_string(&userProcPID))
		{
			HARKLE_ERROR(injector, main, release_a_string failed);
		}
	}

	// 3. 
	if (procMaps_ptr)
	{
		pmparser_free(procMaps_ptr);
		procMaps_ptr = NULL;
	}

	// 4. localBackup
	if (localBackup)
	{
		if (false == free_iovec_struct(&localBackup, true))
		{
			HARKLE_ERROR(injector, main, free_iovec_struct failed);
		}
	}

	// DONE
	return 0;
}
