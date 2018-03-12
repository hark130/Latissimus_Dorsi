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

## TO DO

### 3-3-1
* [X] Open a File Descriptor
* [X] Close a File Descriptor
* [X] Modify a File Descriptor
* [ ] Duplicate a File Descriptor

## NOTES
* File descriptor ranges from 0 to OPEN_MAX
* [/dev/null](https://www.networkworld.com/article/3025497/linux/sending-data-into-the-void-with-dev-null.html)
* Format specifier for uintmax_t: "uintmax_t max   %20ju %16jx\n" /* try PRIuMAX if %ju unsupported */
* Finding the name of a file from a descriptor requires an inode search of the file system, since the operating system only maps descriptors to inodes, not file names.
