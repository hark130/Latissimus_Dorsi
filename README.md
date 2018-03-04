# Latissimus_Dorsi
la·tis·si·mus läˈtisəməs/ nounAnatomy noun: latissimus dorsi      
1.  Either of a pair of large, roughly triangular muscles covering the lower part of the back, extending from the sacral, lumbar, and lower thoracic vertebrae to the armpits.
2.  Latin word for "Lat" or "LAT", an acronym meaning Linux Apprentice Test (LAT).

# 6 Ws 
## What
This is me preparing for an upcoming Linux Apprenctice Test in my relatively near future.  In short, I'm trying to exercise my lats.

## Why
I like to over-prepare, if that's even possible, for tests.  Besides, I like to learn.

## Who
I'm not the only one who would like to prepare, practice, and pontificate.

## Where
I've done work towards this goal before, [Peke Ito](https://github.com/hark130/Peke_Ito), but I wanted all of my effort in one location, organized according to the topic list I have.

## When
In my free time.

## How
* Proof of concept code
* Exercises (overhead pulldown, seated row, pull up, etc)
* Writing tests (What better way to understand material than to write tests for it?)
* Research
* Practice

# Task List
1. Linux Bash
2. Linux Interaction
3. Linux Internals
4. Linux User Mode Development
5. Linux Kernel Development

# Releasable Products

## print_PID_libraries

### NAME

	print_PID_libraries.exe - list files loaded into memory by a PID

### SYNOPSIS

	print_PID_libraries.exe <PID>

### DESCRIPTION

print_PID_libraries.exe will resolve the symbolic links, if any, found in /proc/<PID>/map_files/ to determine what files <PID> has mapped into memory.

If you don't pass an argument to print_PID_libraries.exe, you'll know.  If you don't pass a number, you'll know.  If the PID you choose doesn't exist, you'll know.  If there are no files, you'll know.  If you don't have permissions to access a file, print_PID_libraries.exe will suggest you use 'sudo'.

Output will resolve the PID into its cmdline argument.  It will also list a unique list of files that have been mapped into memory.

If you experience errors and wish to see more <<<ERROR>>> output, uncomment the "HARKLE_DEBUG" MACRO in "./3-Internals/Harklerror.h", build with Make, and run it again.

### COMPILATION

```make print_PID_libraries```

### EXAMPLE

```$ sudo ./print_PID_libraries.exe 4476```

Parsing /proc/4476/ (/usr/sbin/cupsd)
Files loaded by /proc/4476/:

	/usr/sbin/cupsd
	/lib/x86_64-linux-gnu/libnss_files-2.23.so
	[...snip...]
	/lib/x86_64-linux-gnu/ld-2.23.so
	/lib/x86_64-linux-gnu/libsystemd.so.0.14.0
