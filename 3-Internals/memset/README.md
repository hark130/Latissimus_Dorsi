# MEMSET RESEARCH

## BACKGROUND

Reportedly, attempts to memset() memory that is no longer utilized is optimized out by most compilers.

[MSC06-C. Beware of compiler optimizations](https://wiki.sei.cmu.edu/confluence/display/c/MSC06-C.+Beware+of+compiler+optimizations)

## RESEARCH

I wish to find a way to properly (see: compliant code) 'clear' memory before it is released, free()d, or otherwise left behind.

## PLAN

### Situations

I have devised a thorough combination of situations I wish to attempt to attain this goal.  
- Thing - A list of different "things" to attempt to memset
	1. Local scope (stack memory)
	2. Global scope (stack memory)
	3. Heap memory
	4. mmap() memory
- Trick - A list of "tricks" to attempt (e.g., volatile, explicit_bzero)
	1. None
	2. Volatile keyword
	3. pragma optimize()
	4. memset_s()
	5. "do nothing" function
	6. read/write/replace
	7. explicit_bzero()
	8. ["Touching Memory"](https://wiki.sei.cmu.edu/confluence/display/c/MSC06-C.+Beware+of+compiler+optimizations)
	9. Testing harkleset from Memoroad.h
- Object - A list of "objects" to call memset (e.g., function, goto, inline assembly)
	1. Function
	2. Goto label
	3. Inline assembly
- Scheme - A list of compilation schemes to attempt (e.g., linked object code, shared object)
	1. main()
	2. local function()
	3. Header/definition compiled and linked together all at once
	4. Header/definition and main.c compiled separate but linked together
	5. Shared object
	
### Results Sheet
	
Here is the combination of all my ideas into a chart.  That way, I can track my progress and record my results.

[Full experiment results](https://github.com/hark130/Latissimus_Dorsi/blob/memset/3-Internals/memset/20180617-222827_memset_results.md)

[Just experiment success](https://github.com/hark130/Latissimus_Dorsi/blob/memset/3-Internals/memset/20180617-222827_memset_success.md)

### Testing Steps

1. Write and source-control the code file (zeroize-main/header/def-WXYZ.c
	- W == Thing number
	- X == Trick number
	- Y == Object number
	- Z == Scheme number
	- Example: Utilizing inline assembly, with the volatile keyword, that has been compiled into a shared library, on a variable of local scope would use the WXYZ number of 1235.  1 for "local scope", 2 for "volatile", 3 for "assembly", and 5 for "shared object".
2. Add a new recipe to the Makefile (with additional entries for -O1, -O2, and -O3)
3. Add that new recipe to the "all" Makefile recipe
4. ```make all```
5. ```./memset-experiment.exe```
6. Update the "Current experiment results" link above with most recent source-controlled YYYYMMDD-HHMMSS_memset_results.md
