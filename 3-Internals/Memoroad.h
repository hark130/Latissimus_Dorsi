#ifndef __MEMOROAD__
#define __MEMOROAD__

#define _GNU_SOURCE							// process_vm_readv() and process_vm_writev() are only available when GNU extensions are enabled
#include <stdbool.h>						// bool, true, false
#include <stddef.h>							// size_t
#include <sys/mman.h>						// PROT_* MACROS
#include <sys/uio.h>						// struct iovec

/* change_mmap_prot() "newProt" MACRO FLAGS */
// MROAD_PROT_NONE
#ifdef PROT_NONE
#define MROAD_PROT_NONE PROT_NONE
#else
#define MROAD_PROT_NONE 0
#endif  // PROT_NONE

// MROAD_PROT_READ
#ifdef PROT_READ
#define MROAD_PROT_READ PROT_READ
#else
#define MROAD_PROT_READ 0
#endif  // PROT_READ

// MROAD_PROT_WRITE
#ifdef PROT_WRITE
#define MROAD_PROT_WRITE PROT_WRITE
#else
#define MROAD_PROT_WRITE 0
#endif  // PROT_WRITE

// MROAD_PROT_EXEC
#ifdef PROT_EXEC
#define MROAD_PROT_EXEC PROT_EXEC
#else
#define MROAD_PROT_EXEC 0
#endif  // PROT_EXEC

// MROAD_PROT_SEM
#ifdef PROT_SEM
#define MROAD_PROT_SEM PROT_SEM
#else
#define MROAD_PROT_SEM 0
#endif  // PROT_SEM

// MROAD_PROT_SAO
#ifdef PROT_SAO
#define MROAD_PROT_SAO PROT_SAO
#else
#define MROAD_PROT_SAO 0
#endif  // PROT_SAO

// MROAD_PROT_GROWSUP
#ifdef PROT_GROWSUP
#define MROAD_PROT_GROWSUP PROT_GROWSUP
#else
#define MROAD_PROT_GROWSUP 0
#endif  // PROT_GROWSUP

// MROAD_PROT_GROWSDOWN
#ifdef PROT_GROWSDOWN
#define MROAD_PROT_GROWSDOWN PROT_GROWSDOWN
#else
#define MROAD_PROT_GROWSDOWN 0
#endif  // PROT_GROWSDOWN

//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Allocate a buffer of size length
	Input
		length - The length of what you want to store
	Ouput - Heap-allocated, memset, buffer of size length
	Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
		There is no extra space allocated in this buffer
 */
void* get_me_memory(size_t length);


/*
	Purpose - Allocate a buffer of size length + 1
	Input
		length - The length of what you want to store
	Ouput - Heap-allocated, memset, buffer of size length + 1
	Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
 */
char* get_me_a_buffer(size_t length);


/*
	Purpose - Allocate an array of char* on the heap
	Input
		arraySize - Number of char*s to include in the array
		nullTerm - true if this array is to be NULL terminated, 
			false otherwise
	Ouput - Heap-allocated, memset, char* array
	Notes:
        To explain nullTerm another way, if nullTerm is true,
			the actual number of indices allocated will be
			arraySize + 1
 */
char** get_me_a_buffer_array(size_t arraySize, bool nullTerm);


/*
    Purpose - Abstract-away copying char arrays into heap-allocated arrays
    Input
        char_ptr - Nul-terminated character array
    Output - A heap-allocated char array containing a copy of char_ptr
    Notes:
        It is the caller's responsibility to free the char* returned by 
        	this function
 */
char* copy_a_string(const char* char_ptr);


/*
	Purpose - Allocate a pointer to an iovec struct
	Input - None
	Output
		On success, a pointer to a heap-allocated iovec struct
		On failure, NULL
	Notes:
		It is the caller's responsibility to free() this memory
 */
struct iovec* allocate_iovec_struct(void);


//////////////////////////////////////////////////////////////////////////////
///////////////////////// ALLOCATION FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS START ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a heap-allocated char array
	Output - On success, true. Otherwise, false.
	Notes:
		Will attempt to memset, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray);
 */
bool release_a_string(char** charPtr_ptr);


/*
	Purpose - Abstract-away the act of free()ing a heap-allocated char array
	Input
		charPtr_ptr - A pointer to a heap-allocated char array
		buffSize - The size, in memory, of *charPtr_ptr
	Output - On success, true. Otherwise, false.
	Notes:
		Will memset buffSize bytes, free, and NULL "&char_ptr"
		Call this function like this:
			release_a_string(&myCharArray, buffSize);
 */
bool release_a_string_len(char** charPtr_ptr, size_t buffSize);


/*
	Purpose - Ease the suffering of free()ing a char** from parse_proc_PIDs()
	Input
		charArr_ptr - A pointer to an array of char pointers
	Output - true on success, false on failure
	Notes:
		All C-strings will be memset to 0, free()d, and made NULL
		The array of C-strings will then be free()d and made NULL
 */
bool free_char_arr(char*** charArr_ptr);


/*
	Purpose - Free a pointer to an iovec struct
	Input
		oldStruct_ptr - Pointer to a heap-allocated iovec struct pointer
		freeAll - If true, this function will attempt to free() iov_base
	Output
		On success, true
		On failure, false
	Notes:
		This function will zeroize the struct members before free()ing
			the pointer
		This function will not attempt to free iov_base
 */
bool free_iovec_struct(struct iovec** oldStruct_ptr, bool freeAll);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////// FREE FUNCTIONS STOP /////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//////////////////////// MEM TRANSFER FUNCTIONS START ////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Allocate enough memory 'locally' to copy "numBytes" from "pid"s
		memory allocated at remoteMem.  The heap-allocated pointer and size
		of the memory will be returned in a struct iovec pointer.
	Input
		pid - PID from which to copy the memory
		remoteMem - Address from which to start copying
		numBytes - The amount of memory to copy from remoteMem
	Output
		On success, a heap-allocated iovec struct pointer.  iov_base will
			point to heap-allocated memory space containing the information
			copied from remoteMem.  iov_len will be the size of the memory
			allocated at iov_base.
		On failure, NULL
	Notes:
		It is the caller's responsibility to free() the memory located at
			iov_base in addition to free()ing the struct pointer itself
 */
struct iovec* copy_remote_to_local(pid_t pid, void* remoteMem, size_t numBytes);


/*
	Purpose - Copy "numBytes" from localMem to "pid"s memory location at remoteMem
	Input
		pid - PID which own the memory to copy into
		remoteMem - Address to start copying into
		localMem - Address to start copying from
		numBytes - The amount of memory to copy from localMem into remoteMem
	Output
		On success, 0
		On failure, the errno set by the system call to process_vm_writev()
 */
int copy_local_to_remote(pid_t pid, void* remoteMem, void* localMem, size_t numBytes);


//////////////////////////////////////////////////////////////////////////////
//////////////////////// MEM TRANSFER FUNCTIONS STOP /////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
///////////////////////// MAPPED MEM FUNCTIONS START /////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Wrap the function call to mprotect()
	Input
		mem_ptr - Address to the beginning of an mmap'd address range that
			is also aligned to a page boundary
		memLen - The length of mem_ptr in bytes
		newProt - The new protections for mem_ptr.  Use the "newProt" 
			MROAD MACRO FLAGS defined here or utilize the original PROT 
			flags found in <sys/mman.h>
	Output
		On success, 0
		On failure, the errno value returned by mprotect()
 */
int change_mmap_prot(void* mem_ptr, size_t memLen, int newProt);


//////////////////////////////////////////////////////////////////////////////
///////////////////////// MAPPED MEM FUNCTIONS STOP //////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS START ///////////////////////////
//////////////////////////////////////////////////////////////////////////////


/*
	Purpose - Replicate the functionality of the old getpagesize() Linux
		system call by wrapping a call to sysconf()
	Input - None
	Output
		On success, memory page size
		On failure...
			If the limit was indeterminate/unsupported, -2
			All other errors, -1
	Notes:
		If the call to sysconf() fails and PAGE_SIZE is defined in 
			<asm/page.h>, this function will return PAGE_SIZE instead of an
			error code.
 */
long get_page_size(void);


/*
	Purpose - Match a snippet of memory (needle) in a larger 'blob' of memory
	Input
		haystack_ptr - A pointer to a memory area of length haystackLen
		needle_ptr - A pointer to a memory area of length needleLen
		haystackLen - The size of the memory area haystack_ptr points to
		needleLen - The size of the memory area needle_ptr points to
	Output
		On success...
			A pointer to the first occurrence of needle_ptr in haystack_ptr
			-or-
			NULL if needle_ptr is not found in haystack_ptr
		On failure, NULL	
	Notes:
		If this function sounds like strstr() and memcmp() had a child, then you
			understand what I'm trying to do here.
 */
void* mem_hunt(void* haystack_ptr, void* needle_ptr, size_t haystackLen, size_t needleLen);


/*
	Purpose - This function represents an attempt at wrapping memset in
		such a way that it will not get optimized out when memset is
		used on a pointer about to be free()d.
	Input
		s - Memory area to memset
		c - Constant byte to set the memory area "s" to
		n - Number of bytes to set
	Output - A pointer to the memory area "s" on success.
 */
void *harkleset(void* s, int c, size_t n);


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// HELPER FUNCTIONS STOP ////////////////////////////
//////////////////////////////////////////////////////////////////////////////


#endif  // __MEMOROAD__
