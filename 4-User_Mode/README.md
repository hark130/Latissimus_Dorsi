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
* [Yale ELF Format](http://flint.cs.yale.edu/cs422/doc/ELF_Format.pdf)
* [ELF Rundown](https://gist.github.com/CMCDragonkai/10ab53654b2aa6ce55c11cfc5b2432a4)
* [Code Injection into Running Linux Application](https://www.codeproject.com/Articles/33340/Code-Injection-into-Running-Linux-Application)
* [LD_PRELOAD Function Hooking](https://www.technovelty.org/c/using-ld_preload-to-override-a-function.html)
* [Modifying a Dynamic Library Without Changing the Source Code](http://www.linuxjournal.com/article/7795)
* [Magic of LD_PRELOAD for Userland Rootkits](http://fluxius.handgrep.se/2011/10/31/the-magic-of-ld_preload-for-userland-rootkits/)
* [Tutorial - Write a System Call](https://brennan.io/2016/11/14/kernel-dev-ep3/)... sounds like an idea?
* [Reversing with IDA](https://jlospinoso.github.io/developing/software/software%20engineering/reverse%20engineering/assembly/2015/03/06/reversing-with-ida.html)

## TO DO

### 4-5-2
* [X] Load a file into mapped memory
* [X] Determine if that file is an ELF binary
* [X] Determine the file's entry point
* [X] Determine the file's "ELF base address"
* [X] Find the largest 'empty' location to inject code
* [X] Determine the section that holds that 'empty' location
* [X] Make that section executable
* [X] Add some Assembly
* [ ] Dynamically build Assembly
* [X] Execute the new binary

### 4-5-3 (Inject into a running ELF)
* [X] Accomplished in ../3-Internals Makefile recipe 3221 as injector.c

### 4-6-1 (Shady Code Finder)
* [ ] Detect entry point modification
* [ ] Detect control flow hijacking
* [ ] Identify parasite code
* [ ] Check the dynamic segment for DLL injection
* [ ] Identify reverse text padding infections
* [ ] Identify text segment padding infections
* [ ] Identify protected binaries
