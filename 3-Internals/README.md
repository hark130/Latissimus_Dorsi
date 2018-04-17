# 3. LINUX INTERNALS

## DESCRIPTION
Developers will have an in-depth working knowledge of Linux Internals

## TASK LIST
* [ ] 1. Use POSIX system calls using GNU libc (3c)
* [ ] 2. Understand the difference between System Calls and Library Functions (C)
* [X] 3. Open, close, modify, and duplicate File Descriptors (3c)
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

* [File Descriptor Wiki](https://en.wikipedia.org/wiki/File_descriptor)
* [Chapter 1. General Unix and Advanced C - File Descriptors](https://www.bottomupcs.com/file_descriptors.xhtml)
* [nixCraft: Find Out How Many File Descriptors Are Being Used](https://www.cyberciti.biz/tips/linux-procfs-file-descriptors.html)
* [File Descriptors Explained](https://linuxmeerkat.wordpress.com/2011/12/02/file-descriptors-explained/)
* [GNU C Library: File Descriptor Flags](https://www.gnu.org/software/libc/manual/html_node/Descriptor-Flags.html)
* [GNU C Library: File Status Flags](https://www.gnu.org/software/libc/manual/html_node/File-Status-Flags.html)
* [File Descriptor Hijacking](http://phrack.org/issues/51/5.html#article)
* [Hijacking for Fun and Profit](https://www.bignerdranch.com/blog/hijacking-for-fun-and-profit/) ...not sure about this one
* [libc - Low-Level Input/Output](http://kirste.userpage.fu-berlin.de/chemnet/use/info/libc/libc_8.html)
* [Low Level Input/Output](http://kirste.userpage.fu-berlin.de/chemnet/use/info/libc/libc_8.html)... scroll down to [Duplicating Descriptors](http://kirste.userpage.fu-berlin.de/chemnet/use/info/libc/libc_toc.html#TOC139)
* [Inter-Process Pipe-based Communication w/ example](https://linux.die.net/man/2/pipe)
* [Creating Pipes in C](http://tldp.org/LDP/lpg/node11.html)
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
* [Signal handling quiz](http://www.sanfoundry.com/linux-debugging-questions-answers-signal-handling/)
* [How to Write Advanced Signal Handlers in UNIX](http://www.oracle.com/technetwork/articles/servers-storage-dev/signal-handlers-studio-2282526.html)
* [Signal Handling and Nonreentrant Functions](https://www.gnu.org/software/libc/manual/html_node/Nonreentrancy.html)
* [5.4 Blocking signals](https://www.win.tue.nl/~aeb/linux/lk/lk-5.html) may have the solution to a problem with nosig (see: block signals instead of catching them)
* [Fork and how signals are delivered to processes](https://unix.stackexchange.com/questions/176235/fork-and-how-signals-are-delivered-to-processes)
* [nohup](https://en.wikipedia.org/wiki/Nohup) ([also here](https://www.computerhope.com/unix/unohup.htm))... the inspiration for nosig
* [CLI Alternatives to nohup](https://askubuntu.com/questions/600956/alternatives-to-nohup)
* [syscall man page](http://man7.org/linux/man-pages/man2/syscall.2.html)... handy reference extricated from nosig.exe's comments
* [Interfacing Linux Signals](http://syprog.blogspot.com/2011/10/iterfacing-linux-signals.html)... w/ Assembly
* [Synchronization Primitives](http://www.cs.columbia.edu/~hgs/os/sync.html)
* [ncurses](https://www.gnu.org/software/ncurses/)
* [ncurses How To](http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html)
* [ncurses overlapping windows](https://www.gnu.org/software/guile-ncurses/manual/html_node/Overlay-and-manipulate-overlapped-windows.html)
* [Ncurses Programming Guide](www.cs.ukzn.ac.za/~hughm/os/notes/ncurses.html)
* [ncurses Extended ASCII Charts](http://melvilletheatre.com/articles/ncurses-extended-characters/index.html)
* Computation Intensive Examples
	* [Stack Overflow Java answer](https://stackoverflow.com/questions/3693197/cpu-intensive-calculation-examples)
	* [How expensive is an operation on a CPU?](https://streamhpc.com/blog/2012-07-16/how-expensive-is-an-operation-on-a-cpu/)
	* Calculate primes using Newton's method
	* [GPU_loop.c](http://aut.researchgateway.ac.nz/bitstream/handle/10292/5605/FeronB.pdf?sequence=3)
	* [Volatile C loop](https://stackoverflow.com/questions/21161175/example-of-very-cpu-intensive-c-function-under-5-lines-of-code)
	* [Project Euler](https://projecteuler.chat/viewtopic.php?t=1422)
	* [Simple Prime Test](https://en.wikipedia.org/wiki/Primality_test#Simple_methods) starting at a large number and working down
	* [Bailey–Borwein–Plouffe formula](https://en.wikipedia.org/wiki/Bailey%E2%80%93Borwein%E2%80%93Plouffe_formula)
* [memset() optimized out](https://wiki.sei.cmu.edu/confluence/display/c/MSC06-C.+Beware+of+compiler+optimizations)
* Injecting Shell Code into a Process
	* [Code Injection into Running Linux Application](https://www.codeproject.com/Articles/33340/Code-Injection-into-Running-Linux-Application?fid=1534990&df=90&mpp=10&sort=Position&spc=Relaxed&tid=4678205)
	* [ptrace man page](https://linux.die.net/man/2/ptrace)
	* [Injecting code into running process with linux-inject](https://lkubuntu.wordpress.com/2016/01/31/injecting-code-into-running-process-with-linux-inject/)
	* [Linux ptrace introduction AKA injecting into sshd for fun](https://blog.xpnsec.com/linux-process-injection-aka-injecting-into-sshd-for-fun/)
	* [Dynamically Inject a Shared Library Into a Running Process on Android/ARM](https://www.evilsocket.net/2015/05/01/dynamically-inject-a-shared-library-into-a-running-process-on-androidarm/)
	* [Executable Code Injection in Linux](http://www.i-programmer.info/programming/cc/3978-executable-code-injection-in-linux.html?start=1)

## TO DO

### 3-3-1
* [X] Open a File Descriptor
* [X] Close a File Descriptor
* [X] Modify a File Descriptor

### 3-3-2 Duplicate a File Descriptor (redirect_bin_output.exe)
**NOTE:**  This is a continuation of 3-3-2
* [X] Take a CLI command, with flags/options, as an argument
* [X] Based on the binary name and current time, create log names (changing invalid characters to underscores)
	* [X] Parse binary name
	* [X] Get timestamp
	* [X] Create log file name
	* [X] BUG: What if name is all garbage (e.g. %^&)
	* [X] BUG: What if name is a directory (where's os_path_isfile()?)
* [X] Populate struct with filenames for stdout and stderr
* [X] Fork()
* [X] Open() YYYYMMDD-HHMMSS-wrapped_bin-output/errors.txt
* [X] Redirect stdout and stderr to YYYYMMDD-HHMMSS-wrapped_bin-output/errors.txt
* [X] Exec()
* [X] Parent reports on the status of the child
* [X] Glue it all together in redirect_bin_output main()

### 3-3-3 
* [ ] Open a pipe for the redirect_bin_output.exe and the forked binary
* [ ] Allow input to pass from redirect_bin_output.exe along that pipe

#### 3-3 IDEAS:
* [ ] Duplicate a file desc and write to it twice, once for each file descriptor
* [ ] Replace stdin with a different file descriptor (e.g., the read end of a pipe)
* [X] Replace stdout with a different file descriptor (e.g., an actual open() file)
* [X] Replace stderr with a different file descriptor (e.g., an actual open() file)
* [ ] fork() a process to utilize a pipe as the input for a program/command called by exec()
* [X] Write a binary wrapper that automatically redirects certain output to certain places
	* ```command > output.txt```
	* ```command >> output.txt```
	* ```command 2> output.txt```
	* ```command 2>> output.txt```
	* ```command &> output.txt```
	* ```command &>> output.txt```
* [X] Write a function much like runcmd() found [here](https://www.cs.rutgers.edu/~pxk/416/notes/c-tutorials/dup2.html)

## NOTES
* File descriptor ranges from 0 to OPEN_MAX
* [/dev/null](https://www.networkworld.com/article/3025497/linux/sending-data-into-the-void-with-dev-null.html)
* Format specifier for uintmax_t: "uintmax_t max   %20ju %16jx\n" // try PRIuMAX if %ju unsupported
* Finding the name of a file from a descriptor requires an inode search of the file system, since the operating system only maps descriptors to inodes, not file names.

### 3-4-1 

**NOTE:** Write a "wrapper" executable that will ignore certain ignorable signals, similar to the shell utility [nohup](https://en.wikipedia.org/wiki/Nohup)
* [X] Validate argument input
* [X] Verify the file exists?
* [X] Fork
* [X] Call exec*
* [X] Ignore the signals

#### 3-4 IDEAS:

* Program that registers a signal to handle, runs, and allows a user to communicate with it from the CLI via the kill command
* Create a zombie process and then utilize Harkleproc.h to verify the process is a zombie (see: <EMPTY> cmdline file)
* Write a "plug-in" function that would prompt the user for verification if ever the [program received a SIGINT](http://www.csl.mtu.edu/cs4411.choi/www/Resource/signal.pdf) (see: Ctrl-C)
* "Global" signal handler?
* Write a "wrapper" executable that will ignore certain ignorable signals, similar to the shell utility [nohup](https://en.wikipedia.org/wiki/Nohup)
* If a child process makes the call exit(–1), what exit status will be seen by the parent?
* [The Producer / Consumer Problem](https://users.cs.cf.ac.uk/Dave.Marshall/C/node32.html#SECTION003240000000000000000) - Control access of (consumer) reads and (producer) writes to a buffer
* Create an API for a virtual filesystem in memory
	* Calling the entry level function of this library would create a shared pipe with a fork()ed fs manager
	* Back and forth communication could be facilitated across this pipe
	* Control codes indicate certain commands
	* Sample Communication:
		* Are you there?  Go.
		* How many files?  3.
		* Receive a file.  Go.  Transmits file.  Recvd X bytes.

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

### 3-18-1 Monaco Grand Prix Thread Racing

* [ ] Spawn threads with dedicated pipes (F1)
* [ ] Send a signal to the thread group to start (green light)
* [ ] Threads begin process something (ZOOM) to be determined later
* [ ] Main thread begins looping over pipes (track status)
* [ ] Main thread prints status w/ ncurses (track indicator)
* [ ] Continue until the first thread finishes 260.286 km / 161.734 mi (winner) 
* [ ] Send a signal to the thread group to stop (race is over)
* [ ] Report on the race results

### 3-18 IDEAS

* Monaco Grand Prix Thread Racing
	* Spawn a bunch of threads (horses) with dedicated pipes
	* Send a signal to the process group to start (starting gun)
	* Begin looping over pipes (race track status)
	* Update status w/ ncurses (print updating race track)
	* Wait for the first process to finish (winner)
	* Send a signal to the process group to stop (race is over) -or-
	* Wait for the rest to finish (race is over)
	* Visually report on the status of each horse (Mario Kart)

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
