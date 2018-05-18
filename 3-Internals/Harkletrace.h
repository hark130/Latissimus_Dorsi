/*
	REPO:		Latissiumus_Dorsi (https://github.com/hark130/Latissimus_Dorsi)
	FILE:		Harkletrace.h
	PURPOSE:	Helper functions wrapping common uses of the Linux ptrace() system call
	DATE:		20180518
	VERSION:	0.0.1
 */

#ifndef __HARKLETRACE__
#define __HARKLETRACE__


/*
	Purpose - Write a 'blob' to a PID's memory address using ptrace(PTRACE_POKEDATA)
	Input
		pid - The "tracee" PID (see: ptrace(2))
		dest_ptr - Offset into the "tracee"s USER area
		src_ptr - Pointer to the 'blob' being copied in, "word" by "word"
		srcLen - Length of the 'blob'
	Output
		On success, 0
		On failure, errno value returned by ptrace() system call
 */
int htrace_write_data(pid_t pid, void* dest_ptr, void* src_ptr, size_t srcLen);


#endif  // __HARKLETRACE__
