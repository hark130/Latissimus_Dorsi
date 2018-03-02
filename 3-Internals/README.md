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
* [ ] 10. Inspect which modules are loaded in a running application (3c)
* [ ] 11. Allocate, access, and manipulate virtual memory functions using the POSIX API (malloc, ... etc.) (2c)
* [ ] 12. Allocate, access, and manipulate memory-mapped files (2c)
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
* [realpath(3)][http://man7.org/linux/man-pages/man3/realpath.3.html]
* [differences in resolving symlinks between ls, stat, and readlink](https://unix.stackexchange.com/questions/22128/how-to-get-full-path-of-original-file-of-a-soft-symbolic-link)

## TO DO

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
* [ ] Change <EMPTY> cmdline reads to <ZOMBIE> (or something similar).  [Man page](http://man7.org/linux/man-pages/man5/proc.5.html) says empty cmdline files indicate a zombie process.
* [ ] Move bool free_char_arr(char*** charArr_ptr) from Harkleproc to Memoroad where it belongs
* [ ] List the modules loaded in that application
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
* [ ] Which PID would you like to investigate? asd Segmentation fault (core dumped)
* [ ] Seemingly random Segmentation Faults when attempting to access file without elevated permissions
* [ ] Harkledir - populate_hdEnt_struct() - readlink(absPath, symBuf, symBufSize) doesn't seem to like to read more than 64 bytes?!  (strstr truncated buff in /proc/<PID>/maps?  Fix(?) readlink()?)
* [ ] mount shows me that proc has the following options: (rw,nosuid,nodev,noexec,relatime).  Is this why sudo dies on occassion.  Does my binary actually need to run as root?
	* [ ] chmod 4770 print_PID_libraries.exe; chown root:joe print_PID_libraries.exe;  (How bad is this?  Pretty bad, right?)
	* [ ] mount -n -o remount,suid proc as /proc?  (Would this destroy the world?)
	* [ ] Run print_PID_libraries.exe as root (su -, terminal, etc)
* [ ] Crashes on too many entries.  (see: segFault01.txt)  Cut out the middle man?  Skip the structs and go straight for the symbolic link name?  Can I just point at existing names returned by lstat into the struct stat (e.g., d_name)?  realpath(3)?

NOTE:  Big shoutout to ```strace``` for showing me the sudo error that was being silenced.

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


