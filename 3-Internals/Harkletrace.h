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
	Purpose - Read a 'blob' from a PID's memory address using ptrace(PTRACE_PEEKDATA) into
		a heap-allocated buffer
	Input
		pid - The "tracee" PID (see: ptrace(2))
		src_ptr - Offset into the "tracee"s USER area
		srcLen - Length of the 'blob'
		errNum [Out] - Pointer to an integer in which to store errno on error
	Output
		On success, pointer to a heap-allocated buffer containing the 'blob' being read
		On failure, NULL is returned and errno is assigned to errNum
	Notes:
		The void* returned by this function is NOT nul-terminated
		It is the caller's responsibility to free() the memory returned by this function
 */
void* htrace_read_data(pid_t pid, void* src_ptr, size_t srcLen, int* errNum);


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


/*
	Purpose - Match a snippet of memory (needle) in a PID's larger 'blob' of memory
	Input
		pid - The PID of the target process
		haystack_ptr - A pointer to an RVA inside PID's mapped memory that is of length haystackLen
		needle_ptr - A pointer to a memory area of length needleLen
		haystackLen - The size of the memory area haystack_ptr points to
		needleLen - The size of the memory area needle_ptr points to
	Output
		On success...
			Offset to the first occurrence of needle_ptr in haystack_ptr
			-or-
			-1 if needle_ptr is not found in haystack_ptr
		On failure, -2
	Notes:
		If this function sounds like strstr() and memcmp() had a child and then married
			ptrace(PTRACE_POKEDATA), then you understand what I'm trying to do here.
		It is important to note that needle_ptr can NOT be an RVA.  If you want to find an RVA
			snipped within an RVA snipped, read the needle_ptr RVA into a local buffer and then
			call pid_mem_hunt().
 */
size_t pid_mem_hunt(pid_t pid, void* haystack_ptr, void* needle_ptr, size_t haystackLen, size_t needleLen);


#endif  // __HARKLETRACE__
