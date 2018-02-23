#include "Harkledir.h"
#include <stdio.h>
// Initial commit

/*
	1. Programatically determine which applications are running
		1. Terminal
			1. top
			2. htop
			3. ps
			4. pstree
			5. pgrep
		2. Kernel
			1. struct task_struct* task; for_each_process(task) { printk("%s [%d]\n, task->comm, task->pid); }
		3. User Mode
			1. procps (http://procps.sourceforge.net/)
			2. libproc
			3. libprocps
			4. Read the /proc filesystem directly (strace ps -aux)
				(Essentially, loop through all numeric folders in /proc/<pid>, then do readlink 
				/proc/<pid>/exe, or cat /proc/<pid>/cmdline)
				1. List all directories in /proc
				2. Match only the directory names with all numbers
	2. Prompt the user to choose an application
	3. List the modules loaded in that application
		1. ldd - print shared library dependencies
		2. readelf -d $executable | grep 'NEEDED'
		3. /proc/<pid>/maps for running processes (sudo awk '/\.so/{print $6}' /proc/1/maps | sort -u)
		4. strace -e trace=open myprogram
		5. lsof also can show you which libraries are being used for one particular process. (lsof -p 6919|grep mem)
	4. Inspect those modules
	
	NOTE:
		* lsmod - lists all loaded modules
		* modinfo {driver-name} - give more information about a driver
 */
 
 int main(void)
 {
 	// LOCAL VARIABLES
 	dirDetails_ptr test1 = NULL;
 	dirDetails_ptr test2 = NULL;
 	dirDetails_ptr test3 = NULL;

	puts("main() is barely doing something!");

	// TEST 1 - NULL
	test1 = open_dir(NULL);
	if (test1)
	{
		if (test1->dirName)
		{
			fprintf(stdout, "Test 1: dirName == %s\n", test1->dirName);
		}
		else
		{
			fprintf(stderr, "Test 1: ERROR... dirName is NULL!");
		}
	}
	else
	{
		fprintf(stderr, "Test 1: ERROR... struct pointer is NULL!");
	}
	free_dirDetails_ptr(&test1);

	// TEST 2 - Blank string
	test2 = open_dir("");
	if (test2)
	{
		if (test2->dirName)
		{
			fprintf(stdout, "Test 2: dirName == %s\n", test2->dirName);
		}
		else
		{
			fprintf(stderr, "Test 2: ERROR... dirName is NULL!");
		}
	}
	else
	{
		fprintf(stderr, "Test 2: ERROR... struct pointer is NULL!");
	}
	free_dirDetails_ptr(&test2);

	// TEST 3 - Actual directory
	test3 = open_dir("../3-Internals");
	if (test3)
	{
		if (test3->dirName)
		{
			fprintf(stdout, "Test 3: dirName == %s\n", test3->dirName);
		}
		else
		{
			fprintf(stderr, "Test 3: ERROR... dirName is NULL!");
		}
	}
	else
	{
		fprintf(stderr, "Test 3: ERROR... struct pointer is NULL!");
	}
	free_dirDetails_ptr(&test3);

	return 0;
 }
 
