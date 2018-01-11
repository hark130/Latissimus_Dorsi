# 4. LINUX USER MODE DEVELOPMENT

## DESCRIPTION
Developers will apply standard ___ techniques to Linux system programming

## TASK LIST
* [ ] 1. Demonstrate comprehension of how to perform User Mode Operations (2c)
* [X] 2. Develop Position Independent Code (PIC) (3c)
* [ ] 3. Inject Position Independent Code (PIC) (3c)
* [ ] 4. Execute Position Independent Code (PIC) (3c)
* [X] 5. Divert Code Execution (3c)
* [ ] 6. Implement Binary Hardening Techniques (2c)

**NOTE:** See [Air Force Proficiency Codes.jpg](https://github.com/hark130/Latissimus_Dorsi/blob/master/Air%20Force%20Proficiency%20Codes.jpg) for an 'almost' explanation on the 'codes' appending each task list item.

## RESEARCH SOURCES
* [PIC in shared libraries](https://eli.thegreenplace.net/2011/11/03/position-independent-code-pic-in-shared-libraries/)
* Learning Linux Binary Analysis - 2. The ELF Binary Format - ELF relocations - Relocatable code injection-based binary patching (Author: Ryan O'Neill, Pub. Date: Feb 29, 2016)
* [Wrap a libc system call](http://samanbarghi.com/blog/2014/09/05/how-to-wrap-a-system-call-libc-function-in-linux/)
* [Function Hooking - Part I](https://blog.netspi.com/function-hooking-part-i-hooking-shared-library-function-calls-in-linux/) then...
* [Dynamic Linker Tricks](https://rafalcieslak.wordpress.com/2013/04/02/dynamic-linker-tricks-using-ld_preload-to-cheat-inject-features-and-investigate-programs/)
* 4.5. - [Code caves](https://en.wikipedia.org/wiki/Code_cave)
* 4.6. - [Test a binary for PIC](https://unix.stackexchange.com/questions/89211/test-whether-linux-binary-is-compiled-as-position-independent-code)
* 4.6. - [Mitigate ptrace attacks](https://blog.netspi.com/using-strace-to-monitor-ssh-connections-on-linux/)

## TO DO

### 4-5-2
* [X] Load a file into mapped memory
* [X] Determine if that file is an ELF binary
* [X] Determine the file's entry point
* [ ] Change the file's entry point to a function pointer loaded from a shared object

### 4-5-3
* [ ] Load a file into mapped memory
* [ ] Determine if that file is an ELF binary
* [ ] Determine the file's entry point
* [ ] Determine the file's _____ (virtual address offset?)
* [ ] Find the largest 'empty' location to inject code
* [ ] Determine the section that holds that 'empty' location
* [ ] Make that section executable
* [ ] Add some Assembly
* [ ] Execute the new binary
