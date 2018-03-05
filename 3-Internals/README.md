# 3. LINUX INTERNALS

## DESCRIPTION
Developers will have an in-depth working knowledge of Linux Internals

## TASK LIST
* [ ] 1. Use POSIX system calls using GNU libc (3c)
* [ ] 2. Understand the difference between System Calls and Library Functions (C)
* [ ] 3. Open, close, modify, and duplicate File Descriptors (3c)
* [ ] 4. Implement synchronization, memory protection, and shared memory using POSIX primitives and \*nix concepts (3c)
* [ ] 5. Demonstrate Interprocess Communication knowledge by communicating between multiple applications (3c)
* [ ] 6. Handle and send signals between applications (3c)
* [ ] 7. Use process creation APIs to launch and clone processes (3c)
* [ ] 8. Demonstrate understanding of internal structures and purpose of ELF Files (3c)
* [ ] 9. Understand the role and use Shared Objects at runtime (3c)
* [X] 10. Inspect which modules are loaded in a running application (3c)
* [X] 11. Allocate, access, and manipulate virtual memory functions using the POSIX API (malloc, ... etc.) (2c)
* [X] 12. Allocate, access, and manipulate memory-mapped files (2c)
* [ ] 13. Read and manipulate DAC, xattr, and ACL file permissions (2c)
* [ ] 14. Understand and implement the UNIX unique role of files for synchronization and IPC (3c)
* [ ] 15. Demonstrate and understanding of the difference between virtual and physical File Systems (2c)
* [ ] 16. Gain knowledge from the proc and sys VFSs and manipulate the system using them (3c)
* [ ] 17. Understand the role of, interactions between, differences between, and be able to move data between user space and kernel space. (3c)
* [ ] 18. Create a multi-threaded application and use synchronization primitives (3c)
* [ ] 19. Understand the reason for and send ioctols to a kernel device (3c)
* [ ] 20. Use ptrace to manipulate flow control and data in a remote application (3c)
* [ ] 21. Use BSD sockets for network communication (2c)

**NOTE:** See [Air Force Proficiency Codes.jpg](https://github.com/hark130/Latissimus_Dorsi/blob/master/Air%20Force%20Proficiency%20Codes.jpg) for an 'almost' explanation on the 'codes' appending each task list item.

## RESEARCH SOURCES

* [procps - The /proc file system utilities](http://procps.sourceforge.net/)
* [proc(5) man page](http://man7.org/linux/man-pages/man5/proc.5.html)
* [Brief, but good, /proc/PID/maps rundown](https://stackoverflow.com/questions/1401359/understanding-linux-proc-id-maps)
* [Zombie process wiki](https://en.wikipedia.org/wiki/Zombie_process)
* [readlink(2) man page](http://man7.org/linux/man-pages/man2/readlink.2.html) to translate symlinks
* [sys/capability.h example](https://git.kernel.org/pub/scm/linux/kernel/git/morgan/libcap.git/tree/progs/setcap.c)
* [realpath(3)](http://man7.org/linux/man-pages/man3/realpath.3.html)
* [differences in resolving symlinks between ls, stat, and readlink](https://unix.stackexchange.com/questions/22128/how-to-get-full-path-of-original-file-of-a-soft-symbolic-link)
* [Example of using gdb and strace to find the cause of a segmentation fault](http://bl0rg.krunch.be/segfault-gdb-strace.html)
* [github.com/torvalds/linux/signal.h](https://github.com/torvalds/linux/blob/master/include/linux/signal.h)
* [Open Group signal.h](http://pubs.opengroup.org/onlinepubs/009696699/basedefs/signal.h.html)
* [Interesting comments on executing the default signal handler](https://stackoverflow.com/questions/6015498/executing-default-signal-handler)
* [Signal handling primer for glibc 2.2.3](ftp://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_24.html)
* [signal(7) man page](http://man7.org/linux/man-pages/man7/signal.7.html)
* [libc termination signals](https://www.gnu.org/software/libc/manual/html_node/Termination-Signals.html)

## TO DO

### 3-4-1

#### IDEAS:

* Program that registers a signal to handle, runs, and allows a user to communicate with it from the CLI via the kill command
* Create a zombie process and then utilize Harkleproc.h to verify the process is a zombie (see: <EMPTY> cmdline file)
* Write a "plug-in" function that would prompt the user for verification if ever the [program received a SIGINT](http://www.csl.mtu.edu/cs4411.choi/www/Resource/signal.pdf) (see: Ctrl-C)
* "Global" signal handler?
* Write a "wrapper" executable that will ignore certain ignorable signals, similar to the shell utility [nohup](https://en.wikipedia.org/wiki/Nohup)
* If a child process makes the call exit(–1), what exit status will be seen by the parent?

#### NOTES:

* Methods of synchronization between:
	* Parent and child processes
		* fork() and wait()
		* Child exits with 8 bits (e.g., if (childPID == 0) { exit(func(arg)); })
		* Any other interprocess communication method
	* Non-forked processes
		* files
		* file locks
		* semaphores
		* pipes
		* signals
			* sigemptyset() - Initializes the signal set given by set to empty, with all signals excluded from the set.
			* sigaddset() - Add signal signum from set.
			* sigdelset() - Delete signal signum from set.
			* sigprocmask() - Used to fetch and/or change the signal mask of the calling thread. The signal mask is the set of signals whose delivery is currently blocked for the caller.
				* SIG_BLOCK - The set of blocked signals is the union of the current set and the set argument.
				* SIG_UNBLOCK - The signals in set are removed from the current set of blocked signals. It is permissible to attempt to unblock a signal which is not blocked.
				* SIG_SETMASK - The set of blocked signals is set to the argument set.
			* sigsuspend() - Temporarily replaces the signal mask of the calling process with the mask given by mask and then suspends the process until delivery of a signal whose action is to invoke a signal handler or to terminate a process.
			* sigaction() - Allows the calling process to examine and/or specify the action to be associated with a specific signal.
			* kill() - Used to send any signal to any process group or process.
			* raise() - Sends a signal to the calling process or thread.
			* signal() - Use sigaction() instead.
			
		* message passing(?)

### 3-10-1

* [X] Create the framework to facilitate successful completion of 3-10
* [X] Barely test the framework to facilitate successful completion of 3-10

### 3-10-2

* [X] Programatically determine which applications are running
	* [X] Get a list of PIDs
	* [X] Associate those PIDs with applications
		* ```ls -la /proc/<PID>/exe```
		* ```cat /proc/<PID>/cmdline```
		* ```for I in /proc/*/cmdline; do echo $I; cat $I | tr '\000' ' '; echo; done```
* [X] Prompt the user to choose one
* [X] Move bool free_char_arr(char*** charArr_ptr) from Harkleproc to Memoroad where it belongs
* [X] List the modules loaded in that application
	* lsof
		* ```lsof -p <PID>```
		* ```lsof /path/to/lib.so```
		* ```lsof -p <PID> | awk '{print $9}' | grep '\.so'```
	* /proc/maps
		* ```sudo grep lib.so /proc/*/maps```
		* ```cat /proc/<PID>/maps | awk '{print $6}' | grep '\.so' | sort | uniq```
		* Programatically, strstr on "/lib/" or ".so"
		* UPDATE: It should be enough to strstr /proc/PID/maps for ".so"
		* UPDATE: **Best Idea** Use the list of filenames from /proc/<PID>/map_files to strstr /proc/<PID>/maps
	* ```strace CMD.... 2>&1 | grep '^open(".*\.so"'```
	* ```ltrace```
	* ```ldd```
	* ```objdump -p /path/to/program | grep NEEDED```
	* ```pldd```
	* ```pmap```
	* ```nm```

**BUGS:**

* [X] All closed

NOTE:  Big shoutout to ```strace``` for showing me the sudo error that was being silenced and ```valgrind``` for bringing to light my memory leaks.
```valgrind -v --leak-check=full --track-origins=yes ./print_PID_libraries.exe 3549```

### 3-10-3

**NOTE:** This is a continuation of 3-10-2

* [ ] Parse the ELF of a PID's application to determine "NEEDED" libraries
* [ ] Allow the user to visually compare the differences

### 3-10-4

**NOTE:** This is a continuation of 3-10-3

* [X] Extricate numerous Linux file IO functions into their own header (Completed in 3-10-2... I couldn't wait)
* [X] Write functions to resolve symbolic links, programatically (Completed in 3-10-2... doesn't work so well in /proc though)
* [ ] Draw the complete line of a program's loaded libraries by
	* [ ] Printing the "NEEDED" libraries from an application's ELF Header
	* [ ] Use the above file IO functionality to resolve those (inevitably) symbolic links to their destinations
	* [ ] Print the actual libraries loaded in /proc/<PID>/mem

### 3-11

* [X] See Memoroad.h

### 3-12

* [X] See 4-User_Mode/Map_Memory.h 

### NOTES

* From proc(5) man page:
/proc/[pid]/cmdline
              This read-only file holds the complete command line for the
              process, **unless the process is a zombie.  In the latter case,
              there is nothing in this file: that is, a read on this file
              will return 0 characters.**  The command-line arguments appear
              in this file as a set of strings separated by null bytes
              ('\0'), with a further null byte after the last string.
* ```sudo setcap cap_sys_admin+ep 3-10_Proc_Walk-2_main.exe```


