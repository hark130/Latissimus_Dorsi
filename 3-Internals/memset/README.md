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

| Source Filename | Thing         | Trick                   | Object          | Scheme            | Results |
| :-------------- | :-----------: | :---------------------: | :-------------: | :---------------: | :-----: |
| memset-1111.c   | Local scope   | None                    | Function        | main()            | 
| memset-2111.c   | Global scope  | None                    | Function        | main()            | 
| memset-3111.c   | Heap memory   | None                    | Function        | main()            | 
|                 | mmap() memory | None                    | Function        | main()            | 
| memset-1211.c   | Local scope   | Volatile                | Function        | main()            |
| memset-2211.c   | Global scope  | Volatile                | Function        | main()            |
| memset-3211.c   | Heap memory   | Volatile                | Function        | main()            |
|                 | mmap() memory | Volatile                | Function        | main()            |
|                 | Local scope   | pragma                  | Function        | main()            |
|                 | Global scope  | pragma                  | Function        | main()            |
|                 | Heap memory   | pragma                  | Function        | main()            |
|                 | mmap() memory | pragma                  | Function        | main()            |
|                 | Local scope   | memset_s()              | Function        | main()            |
|                 | Global scope  | memset_s()              | Function        | main()            |
|                 | Heap memory   | memset_s()              | Function        | main()            |
|                 | mmap() memory | memset_s()              | Function        | main()            |
|                 | Local scope   | pass to do-nothing func | Function        | main()            |
|                 | Global scope  | pass to do-nothing func | Function        | main()            |
|                 | Heap memory   | pass to do-nothing func | Function        | main()            |
|                 | mmap() memory | pass to do-nothing func | Function        | main()            |
|                 | Local scope   | read/write/replace      | Function        | main()            |
|                 | Global scope  | read/write/replace      | Function        | main()            |
|                 | Heap memory   | read/write/replace      | Function        | main()            |
|                 | mmap() memory | read/write/replace      | Function        | main()            |
|                 | Local scope   | explicit_bzero          | Function        | main()            |
|                 | Global scope  | explicit_bzero          | Function        | main()            |
|                 | Heap memory   | explicit_bzero          | Function        | main()            |
|                 | mmap() memory | explicit_bzero          | Function        | main()            |
|                 | Local scope   | None                    | Goto            | main()            |
|                 | Global scope  | None                    | Goto            | main()            |
|                 | Heap memory   | None                    | Goto            | main()            |
|                 | mmap() memory | None                    | Goto            | main()            |
|                 | Local scope   | Volatile                | Goto            | main()            |
|                 | Global scope  | Volatile                | Goto            | main()            |
|                 | Heap memory   | Volatile                | Goto            | main()            |
|                 | mmap() memory | Volatile                | Goto            | main()            |
|                 | Local scope   | pragma                  | Goto            | main()            |
|                 | Global scope  | pragma                  | Goto            | main()            |
|                 | Heap memory   | pragma                  | Goto            | main()            |
|                 | mmap() memory | pragma                  | Goto            | main()            |
|                 | Local scope   | memset_s()              | Goto            | main()            |
|                 | Global scope  | memset_s()              | Goto            | main()            |
|                 | Heap memory   | memset_s()              | Goto            | main()            |
|                 | mmap() memory | memset_s()              | Goto            | main()            |
|                 | Local scope   | pass to do-nothing func | Goto            | main()            |
|                 | Global scope  | pass to do-nothing func | Goto            | main()            |
|                 | Heap memory   | pass to do-nothing func | Goto            | main()            |
|                 | mmap() memory | pass to do-nothing func | Goto            | main()            |
|                 | Local scope   | read/write/replace      | Goto            | main()            |
|                 | Global scope  | read/write/replace      | Goto            | main()            |
|                 | Heap memory   | read/write/replace      | Goto            | main()            |
|                 | mmap() memory | read/write/replace      | Goto            | main()            |
|                 | Local scope   | explicit_bzero          | Goto            | main()            |
|                 | Global scope  | explicit_bzero          | Goto            | main()            |
|                 | Heap memory   | explicit_bzero          | Goto            | main()            |
|                 | mmap() memory | explicit_bzero          | Goto            | main()            |
|                 | Local scope   | None                    | Inline Assembly | main()            |
|                 | Global scope  | None                    | Inline Assembly | main()            |
|                 | Heap memory   | None                    | Inline Assembly | main()            |
|                 | mmap() memory | None                    | Inline Assembly | main()            |
|                 | Local scope   | Volatile                | Inline Assembly | main()            |
|                 | Global scope  | Volatile                | Inline Assembly | main()            |
|                 | Heap memory   | Volatile                | Inline Assembly | main()            |
|                 | mmap() memory | Volatile                | Inline Assembly | main()            |
|                 | Local scope   | pragma                  | Inline Assembly | main()            |
|                 | Global scope  | pragma                  | Inline Assembly | main()            |
|                 | Heap memory   | pragma                  | Inline Assembly | main()            |
|                 | mmap() memory | pragma                  | Inline Assembly | main()            |
|                 | Local scope   | memset_s()              | Inline Assembly | main()            |
|                 | Global scope  | memset_s()              | Inline Assembly | main()            |
|                 | Heap memory   | memset_s()              | Inline Assembly | main()            |
|                 | mmap() memory | memset_s()              | Inline Assembly | main()            |
|                 | Local scope   | pass to do-nothing func | Inline Assembly | main()            |
|                 | Global scope  | pass to do-nothing func | Inline Assembly | main()            |
|                 | Heap memory   | pass to do-nothing func | Inline Assembly | main()            |
|                 | mmap() memory | pass to do-nothing func | Inline Assembly | main()            |
|                 | Local scope   | read/write/replace      | Inline Assembly | main()            |
|                 | Global scope  | read/write/replace      | Inline Assembly | main()            |
|                 | Heap memory   | read/write/replace      | Inline Assembly | main()            |
|                 | mmap() memory | read/write/replace      | Inline Assembly | main()            |
|                 | Local scope   | explicit_bzero          | Inline Assembly | main()            |
|                 | Global scope  | explicit_bzero          | Inline Assembly | main()            |
|                 | Heap memory   | explicit_bzero          | Inline Assembly | main()            |
|                 | mmap() memory | explicit_bzero          | Inline Assembly | main()            |
|                 | Local scope   | None                    | Function        | Local func()      |
|                 | Global scope  | None                    | Function        | Local func()      |
|                 | Heap memory   | None                    | Function        | Local func()      |
|                 | mmap() memory | None                    | Function        | Local func()      |
|                 | Local scope   | Volatile                | Function        | Local func()      |
|                 | Global scope  | Volatile                | Function        | Local func()      |
|                 | Heap memory   | Volatile                | Function        | Local func()      |
|                 | mmap() memory | Volatile                | Function        | Local func()      |
|                 | Local scope   | pragma                  | Function        | Local func()      |
|                 | Global scope  | pragma                  | Function        | Local func()      |
|                 | Heap memory   | pragma                  | Function        | Local func()      |
|                 | mmap() memory | pragma                  | Function        | Local func()      |
|                 | Local scope   | memset_s()              | Function        | Local func()      |
|                 | Global scope  | memset_s()              | Function        | Local func()      |
|                 | Heap memory   | memset_s()              | Function        | Local func()      |
|                 | mmap() memory | memset_s()              | Function        | Local func()      |
|                 | Local scope   | pass to do-nothing func | Function        | Local func()      |
|                 | Global scope  | pass to do-nothing func | Function        | Local func()      |
|                 | Heap memory   | pass to do-nothing func | Function        | Local func()      |
|                 | mmap() memory | pass to do-nothing func | Function        | Local func()      |
|                 | Local scope   | read/write/replace      | Function        | Local func()      |
|                 | Global scope  | read/write/replace      | Function        | Local func()      |
|                 | Heap memory   | read/write/replace      | Function        | Local func()      |
|                 | mmap() memory | read/write/replace      | Function        | Local func()      |
|                 | Local scope   | explicit_bzero          | Function        | Local func()      |
|                 | Global scope  | explicit_bzero          | Function        | Local func()      |
|                 | Heap memory   | explicit_bzero          | Function        | Local func()      |
|                 | mmap() memory | explicit_bzero          | Function        | Local func()      |
|                 | Local scope   | None                    | Goto            | Local func()      |
|                 | Global scope  | None                    | Goto            | Local func()      |
|                 | Heap memory   | None                    | Goto            | Local func()      |
|                 | mmap() memory | None                    | Goto            | Local func()      |
|                 | Local scope   | Volatile                | Goto            | Local func()      |
|                 | Global scope  | Volatile                | Goto            | Local func()      |
|                 | Heap memory   | Volatile                | Goto            | Local func()      |
|                 | mmap() memory | Volatile                | Goto            | Local func()      |
|                 | Local scope   | pragma                  | Goto            | Local func()      |
|                 | Global scope  | pragma                  | Goto            | Local func()      |
|                 | Heap memory   | pragma                  | Goto            | Local func()      |
|                 | mmap() memory | pragma                  | Goto            | Local func()      |
|                 | Local scope   | memset_s()              | Goto            | Local func()      |
|                 | Global scope  | memset_s()              | Goto            | Local func()      |
|                 | Heap memory   | memset_s()              | Goto            | Local func()      |
|                 | mmap() memory | memset_s()              | Goto            | Local func()      |
|                 | Local scope   | pass to do-nothing func | Goto            | Local func()      |
|                 | Global scope  | pass to do-nothing func | Goto            | Local func()      |
|                 | Heap memory   | pass to do-nothing func | Goto            | Local func()      |
|                 | mmap() memory | pass to do-nothing func | Goto            | Local func()      |
|                 | Local scope   | read/write/replace      | Goto            | Local func()      |
|                 | Global scope  | read/write/replace      | Goto            | Local func()      |
|                 | Heap memory   | read/write/replace      | Goto            | Local func()      |
|                 | mmap() memory | read/write/replace      | Goto            | Local func()      |
|                 | Local scope   | explicit_bzero          | Goto            | Local func()      |
|                 | Global scope  | explicit_bzero          | Goto            | Local func()      |
|                 | Heap memory   | explicit_bzero          | Goto            | Local func()      |
|                 | mmap() memory | explicit_bzero          | Goto            | Local func()      |
|                 | Local scope   | None                    | Inline Assembly | Local func()      |
|                 | Global scope  | None                    | Inline Assembly | Local func()      |
|                 | Heap memory   | None                    | Inline Assembly | Local func()      |
|                 | mmap() memory | None                    | Inline Assembly | Local func()      |
|                 | Local scope   | Volatile                | Inline Assembly | Local func()      |
|                 | Global scope  | Volatile                | Inline Assembly | Local func()      |
|                 | Heap memory   | Volatile                | Inline Assembly | Local func()      |
|                 | mmap() memory | Volatile                | Inline Assembly | Local func()      |
|                 | Local scope   | pragma                  | Inline Assembly | Local func()      |
|                 | Global scope  | pragma                  | Inline Assembly | Local func()      |
|                 | Heap memory   | pragma                  | Inline Assembly | Local func()      |
|                 | mmap() memory | pragma                  | Inline Assembly | Local func()      |
|                 | Local scope   | memset_s()              | Inline Assembly | Local func()      |
|                 | Global scope  | memset_s()              | Inline Assembly | Local func()      |
|                 | Heap memory   | memset_s()              | Inline Assembly | Local func()      |
|                 | mmap() memory | memset_s()              | Inline Assembly | Local func()      |
|                 | Local scope   | pass to do-nothing func | Inline Assembly | Local func()      |
|                 | Global scope  | pass to do-nothing func | Inline Assembly | Local func()      |
|                 | Heap memory   | pass to do-nothing func | Inline Assembly | Local func()      |
|                 | mmap() memory | pass to do-nothing func | Inline Assembly | Local func()      |
|                 | Local scope   | read/write/replace      | Inline Assembly | Local func()      |
|                 | Global scope  | read/write/replace      | Inline Assembly | Local func()      |
|                 | Heap memory   | read/write/replace      | Inline Assembly | Local func()      |
|                 | mmap() memory | read/write/replace      | Inline Assembly | Local func()      |
|                 | Local scope   | explicit_bzero          | Inline Assembly | Local func()      |
|                 | Global scope  | explicit_bzero          | Inline Assembly | Local func()      |
|                 | Heap memory   | explicit_bzero          | Inline Assembly | Local func()      |
|                 | mmap() memory | explicit_bzero          | Inline Assembly | Local func()      |
|                 | Local scope   | None                    | Function        | Compiled Together |
|                 | Global scope  | None                    | Function        | Compiled Together |
|                 | Heap memory   | None                    | Function        | Compiled Together |
|                 | mmap() memory | None                    | Function        | Compiled Together |
|                 | Local scope   | Volatile                | Function        | Compiled Together |
|                 | Global scope  | Volatile                | Function        | Compiled Together |
|                 | Heap memory   | Volatile                | Function        | Compiled Together |
|                 | mmap() memory | Volatile                | Function        | Compiled Together |
|                 | Local scope   | pragma                  | Function        | Compiled Together |
|                 | Global scope  | pragma                  | Function        | Compiled Together |
|                 | Heap memory   | pragma                  | Function        | Compiled Together |
|                 | mmap() memory | pragma                  | Function        | Compiled Together |
|                 | Local scope   | memset_s()              | Function        | Compiled Together |
|                 | Global scope  | memset_s()              | Function        | Compiled Together |
|                 | Heap memory   | memset_s()              | Function        | Compiled Together |
|                 | mmap() memory | memset_s()              | Function        | Compiled Together |
|                 | Local scope   | pass to do-nothing func | Function        | Compiled Together |
|                 | Global scope  | pass to do-nothing func | Function        | Compiled Together |
|                 | Heap memory   | pass to do-nothing func | Function        | Compiled Together |
|                 | mmap() memory | pass to do-nothing func | Function        | Compiled Together |
|                 | Local scope   | read/write/replace      | Function        | Compiled Together |
|                 | Global scope  | read/write/replace      | Function        | Compiled Together |
|                 | Heap memory   | read/write/replace      | Function        | Compiled Together |
|                 | mmap() memory | read/write/replace      | Function        | Compiled Together |
|                 | Local scope   | explicit_bzero          | Function        | Compiled Together |
|                 | Global scope  | explicit_bzero          | Function        | Compiled Together |
|                 | Heap memory   | explicit_bzero          | Function        | Compiled Together |
|                 | mmap() memory | explicit_bzero          | Function        | Compiled Together |
|                 | Local scope   | None                    | Goto            | Compiled Together |
|                 | Global scope  | None                    | Goto            | Compiled Together |
|                 | Heap memory   | None                    | Goto            | Compiled Together |
|                 | mmap() memory | None                    | Goto            | Compiled Together |
|                 | Local scope   | Volatile                | Goto            | Compiled Together |
|                 | Global scope  | Volatile                | Goto            | Compiled Together |
|                 | Heap memory   | Volatile                | Goto            | Compiled Together |
|                 | mmap() memory | Volatile                | Goto            | Compiled Together |
|                 | Local scope   | pragma                  | Goto            | Compiled Together |
|                 | Global scope  | pragma                  | Goto            | Compiled Together |
|                 | Heap memory   | pragma                  | Goto            | Compiled Together |
|                 | mmap() memory | pragma                  | Goto            | Compiled Together |
|                 | Local scope   | memset_s()              | Goto            | Compiled Together |
|                 | Global scope  | memset_s()              | Goto            | Compiled Together |
|                 | Heap memory   | memset_s()              | Goto            | Compiled Together |
|                 | mmap() memory | memset_s()              | Goto            | Compiled Together |
|                 | Local scope   | pass to do-nothing func | Goto            | Compiled Together |
|                 | Global scope  | pass to do-nothing func | Goto            | Compiled Together |
|                 | Heap memory   | pass to do-nothing func | Goto            | Compiled Together |
|                 | mmap() memory | pass to do-nothing func | Goto            | Compiled Together |
|                 | Local scope   | read/write/replace      | Goto            | Compiled Together |
|                 | Global scope  | read/write/replace      | Goto            | Compiled Together |
|                 | Heap memory   | read/write/replace      | Goto            | Compiled Together |
|                 | mmap() memory | read/write/replace      | Goto            | Compiled Together |
|                 | Local scope   | explicit_bzero          | Goto            | Compiled Together |
|                 | Global scope  | explicit_bzero          | Goto            | Compiled Together |
|                 | Heap memory   | explicit_bzero          | Goto            | Compiled Together |
|                 | mmap() memory | explicit_bzero          | Goto            | Compiled Together |
|                 | Local scope   | None                    | Inline Assembly | Compiled Together |
|                 | Global scope  | None                    | Inline Assembly | Compiled Together |
|                 | Heap memory   | None                    | Inline Assembly | Compiled Together |
|                 | mmap() memory | None                    | Inline Assembly | Compiled Together |
|                 | Local scope   | Volatile                | Inline Assembly | Compiled Together |
|                 | Global scope  | Volatile                | Inline Assembly | Compiled Together |
|                 | Heap memory   | Volatile                | Inline Assembly | Compiled Together |
|                 | mmap() memory | Volatile                | Inline Assembly | Compiled Together |
|                 | Local scope   | pragma                  | Inline Assembly | Compiled Together |
|                 | Global scope  | pragma                  | Inline Assembly | Compiled Together |
|                 | Heap memory   | pragma                  | Inline Assembly | Compiled Together |
|                 | mmap() memory | pragma                  | Inline Assembly | Compiled Together |
|                 | Local scope   | memset_s()              | Inline Assembly | Compiled Together |
|                 | Global scope  | memset_s()              | Inline Assembly | Compiled Together |
|                 | Heap memory   | memset_s()              | Inline Assembly | Compiled Together |
|                 | mmap() memory | memset_s()              | Inline Assembly | Compiled Together |
|                 | Local scope   | pass to do-nothing func | Inline Assembly | Compiled Together |
|                 | Global scope  | pass to do-nothing func | Inline Assembly | Compiled Together |
|                 | Heap memory   | pass to do-nothing func | Inline Assembly | Compiled Together |
|                 | mmap() memory | pass to do-nothing func | Inline Assembly | Compiled Together |
|                 | Local scope   | read/write/replace      | Inline Assembly | Compiled Together |
|                 | Global scope  | read/write/replace      | Inline Assembly | Compiled Together |
|                 | Heap memory   | read/write/replace      | Inline Assembly | Compiled Together |
|                 | mmap() memory | read/write/replace      | Inline Assembly | Compiled Together |
|                 | Local scope   | explicit_bzero          | Inline Assembly | Compiled Together |
|                 | Global scope  | explicit_bzero          | Inline Assembly | Compiled Together |
|                 | Heap memory   | explicit_bzero          | Inline Assembly | Compiled Together |
|                 | mmap() memory | explicit_bzero          | Inline Assembly | Compiled Together |
|                 | Local scope   | None                    | Function        | Linked Together   |
|                 | Global scope  | None                    | Function        | Linked Together   |
|                 | Heap memory   | None                    | Function        | Linked Together   |
|                 | mmap() memory | None                    | Function        | Linked Together   |
|                 | Local scope   | Volatile                | Function        | Linked Together   |
|                 | Global scope  | Volatile                | Function        | Linked Together   |
|                 | Heap memory   | Volatile                | Function        | Linked Together   |
|                 | mmap() memory | Volatile                | Function        | Linked Together   |
|                 | Local scope   | pragma                  | Function        | Linked Together   |
|                 | Global scope  | pragma                  | Function        | Linked Together   |
|                 | Heap memory   | pragma                  | Function        | Linked Together   |
|                 | mmap() memory | pragma                  | Function        | Linked Together   |
|                 | Local scope   | memset_s()              | Function        | Linked Together   |
|                 | Global scope  | memset_s()              | Function        | Linked Together   |
|                 | Heap memory   | memset_s()              | Function        | Linked Together   |
|                 | mmap() memory | memset_s()              | Function        | Linked Together   |
|                 | Local scope   | pass to do-nothing func | Function        | Linked Together   |
|                 | Global scope  | pass to do-nothing func | Function        | Linked Together   |
|                 | Heap memory   | pass to do-nothing func | Function        | Linked Together   |
|                 | mmap() memory | pass to do-nothing func | Function        | Linked Together   |
|                 | Local scope   | read/write/replace      | Function        | Linked Together   |
|                 | Global scope  | read/write/replace      | Function        | Linked Together   |
|                 | Heap memory   | read/write/replace      | Function        | Linked Together   |
|                 | mmap() memory | read/write/replace      | Function        | Linked Together   |
|                 | Local scope   | explicit_bzero          | Function        | Linked Together   |
|                 | Global scope  | explicit_bzero          | Function        | Linked Together   |
|                 | Heap memory   | explicit_bzero          | Function        | Linked Together   |
|                 | mmap() memory | explicit_bzero          | Function        | Linked Together   |
|                 | Local scope   | None                    | Goto            | Linked Together   |
|                 | Global scope  | None                    | Goto            | Linked Together   |
|                 | Heap memory   | None                    | Goto            | Linked Together   |
|                 | mmap() memory | None                    | Goto            | Linked Together   |
|                 | Local scope   | Volatile                | Goto            | Linked Together   |
|                 | Global scope  | Volatile                | Goto            | Linked Together   |
|                 | Heap memory   | Volatile                | Goto            | Linked Together   |
|                 | mmap() memory | Volatile                | Goto            | Linked Together   |
|                 | Local scope   | pragma                  | Goto            | Linked Together   |
|                 | Global scope  | pragma                  | Goto            | Linked Together   |
|                 | Heap memory   | pragma                  | Goto            | Linked Together   |
|                 | mmap() memory | pragma                  | Goto            | Linked Together   |
|                 | Local scope   | memset_s()              | Goto            | Linked Together   |
|                 | Global scope  | memset_s()              | Goto            | Linked Together   |
|                 | Heap memory   | memset_s()              | Goto            | Linked Together   |
|                 | mmap() memory | memset_s()              | Goto            | Linked Together   |
|                 | Local scope   | pass to do-nothing func | Goto            | Linked Together   |
|                 | Global scope  | pass to do-nothing func | Goto            | Linked Together   |
|                 | Heap memory   | pass to do-nothing func | Goto            | Linked Together   |
|                 | mmap() memory | pass to do-nothing func | Goto            | Linked Together   |
|                 | Local scope   | read/write/replace      | Goto            | Linked Together   |
|                 | Global scope  | read/write/replace      | Goto            | Linked Together   |
|                 | Heap memory   | read/write/replace      | Goto            | Linked Together   |
|                 | mmap() memory | read/write/replace      | Goto            | Linked Together   |
|                 | Local scope   | explicit_bzero          | Goto            | Linked Together   |
|                 | Global scope  | explicit_bzero          | Goto            | Linked Together   |
|                 | Heap memory   | explicit_bzero          | Goto            | Linked Together   |
|                 | mmap() memory | explicit_bzero          | Goto            | Linked Together   |
|                 | Local scope   | None                    | Inline Assembly | Linked Together   |
|                 | Global scope  | None                    | Inline Assembly | Linked Together   |
|                 | Heap memory   | None                    | Inline Assembly | Linked Together   |
|                 | mmap() memory | None                    | Inline Assembly | Linked Together   |
|                 | Local scope   | Volatile                | Inline Assembly | Linked Together   |
|                 | Global scope  | Volatile                | Inline Assembly | Linked Together   |
|                 | Heap memory   | Volatile                | Inline Assembly | Linked Together   |
|                 | mmap() memory | Volatile                | Inline Assembly | Linked Together   |
|                 | Local scope   | pragma                  | Inline Assembly | Linked Together   |
|                 | Global scope  | pragma                  | Inline Assembly | Linked Together   |
|                 | Heap memory   | pragma                  | Inline Assembly | Linked Together   |
|                 | mmap() memory | pragma                  | Inline Assembly | Linked Together   |
|                 | Local scope   | memset_s()              | Inline Assembly | Linked Together   |
|                 | Global scope  | memset_s()              | Inline Assembly | Linked Together   |
|                 | Heap memory   | memset_s()              | Inline Assembly | Linked Together   |
|                 | mmap() memory | memset_s()              | Inline Assembly | Linked Together   |
|                 | Local scope   | pass to do-nothing func | Inline Assembly | Linked Together   |
|                 | Global scope  | pass to do-nothing func | Inline Assembly | Linked Together   |
|                 | Heap memory   | pass to do-nothing func | Inline Assembly | Linked Together   |
|                 | mmap() memory | pass to do-nothing func | Inline Assembly | Linked Together   |
|                 | Local scope   | read/write/replace      | Inline Assembly | Linked Together   |
|                 | Global scope  | read/write/replace      | Inline Assembly | Linked Together   |
|                 | Heap memory   | read/write/replace      | Inline Assembly | Linked Together   |
|                 | mmap() memory | read/write/replace      | Inline Assembly | Linked Together   |
|                 | Local scope   | explicit_bzero          | Inline Assembly | Linked Together   |
|                 | Global scope  | explicit_bzero          | Inline Assembly | Linked Together   |
|                 | Heap memory   | explicit_bzero          | Inline Assembly | Linked Together   |
|                 | mmap() memory | explicit_bzero          | Inline Assembly | Linked Together   |
|                 | Local scope   | None                    | Function        | Shared Object     |
|                 | Global scope  | None                    | Function        | Shared Object     |
|                 | Heap memory   | None                    | Function        | Shared Object     |
|                 | mmap() memory | None                    | Function        | Shared Object     |
|                 | Local scope   | Volatile                | Function        | Shared Object     |
|                 | Global scope  | Volatile                | Function        | Shared Object     |
|                 | Heap memory   | Volatile                | Function        | Shared Object     |
|                 | mmap() memory | Volatile                | Function        | Shared Object     |
|                 | Local scope   | pragma                  | Function        | Shared Object     |
|                 | Global scope  | pragma                  | Function        | Shared Object     |
|                 | Heap memory   | pragma                  | Function        | Shared Object     |
|                 | mmap() memory | pragma                  | Function        | Shared Object     |
|                 | Local scope   | memset_s()              | Function        | Shared Object     |
|                 | Global scope  | memset_s()              | Function        | Shared Object     |
|                 | Heap memory   | memset_s()              | Function        | Shared Object     |
|                 | mmap() memory | memset_s()              | Function        | Shared Object     |
|                 | Local scope   | pass to do-nothing func | Function        | Shared Object     |
|                 | Global scope  | pass to do-nothing func | Function        | Shared Object     |
|                 | Heap memory   | pass to do-nothing func | Function        | Shared Object     |
|                 | mmap() memory | pass to do-nothing func | Function        | Shared Object     |
|                 | Local scope   | read/write/replace      | Function        | Shared Object     |
|                 | Global scope  | read/write/replace      | Function        | Shared Object     |
|                 | Heap memory   | read/write/replace      | Function        | Shared Object     |
|                 | mmap() memory | read/write/replace      | Function        | Shared Object     |
|                 | Local scope   | explicit_bzero          | Function        | Shared Object     |
|                 | Global scope  | explicit_bzero          | Function        | Shared Object     |
|                 | Heap memory   | explicit_bzero          | Function        | Shared Object     |
|                 | mmap() memory | explicit_bzero          | Function        | Shared Object     |
|                 | Local scope   | None                    | Goto            | Shared Object     |
|                 | Global scope  | None                    | Goto            | Shared Object     |
|                 | Heap memory   | None                    | Goto            | Shared Object     |
|                 | mmap() memory | None                    | Goto            | Shared Object     |
|                 | Local scope   | Volatile                | Goto            | Shared Object     |
|                 | Global scope  | Volatile                | Goto            | Shared Object     |
|                 | Heap memory   | Volatile                | Goto            | Shared Object     |
|                 | mmap() memory | Volatile                | Goto            | Shared Object     |
|                 | Local scope   | pragma                  | Goto            | Shared Object     |
|                 | Global scope  | pragma                  | Goto            | Shared Object     |
|                 | Heap memory   | pragma                  | Goto            | Shared Object     |
|                 | mmap() memory | pragma                  | Goto            | Shared Object     |
|                 | Local scope   | memset_s()              | Goto            | Shared Object     |
|                 | Global scope  | memset_s()              | Goto            | Shared Object     |
|                 | Heap memory   | memset_s()              | Goto            | Shared Object     |
|                 | mmap() memory | memset_s()              | Goto            | Shared Object     |
|                 | Local scope   | pass to do-nothing func | Goto            | Shared Object     |
|                 | Global scope  | pass to do-nothing func | Goto            | Shared Object     |
|                 | Heap memory   | pass to do-nothing func | Goto            | Shared Object     |
|                 | mmap() memory | pass to do-nothing func | Goto            | Shared Object     |
|                 | Local scope   | read/write/replace      | Goto            | Shared Object     |
|                 | Global scope  | read/write/replace      | Goto            | Shared Object     |
|                 | Heap memory   | read/write/replace      | Goto            | Shared Object     |
|                 | mmap() memory | read/write/replace      | Goto            | Shared Object     |
|                 | Local scope   | explicit_bzero          | Goto            | Shared Object     |
|                 | Global scope  | explicit_bzero          | Goto            | Shared Object     |
|                 | Heap memory   | explicit_bzero          | Goto            | Shared Object     |
|                 | mmap() memory | explicit_bzero          | Goto            | Shared Object     |
|                 | Local scope   | None                    | Inline Assembly | Shared Object     |
|                 | Global scope  | None                    | Inline Assembly | Shared Object     |
|                 | Heap memory   | None                    | Inline Assembly | Shared Object     |
|                 | mmap() memory | None                    | Inline Assembly | Shared Object     |
|                 | Local scope   | Volatile                | Inline Assembly | Shared Object     |
|                 | Global scope  | Volatile                | Inline Assembly | Shared Object     |
|                 | Heap memory   | Volatile                | Inline Assembly | Shared Object     |
|                 | mmap() memory | Volatile                | Inline Assembly | Shared Object     |
|                 | Local scope   | pragma                  | Inline Assembly | Shared Object     |
|                 | Global scope  | pragma                  | Inline Assembly | Shared Object     |
|                 | Heap memory   | pragma                  | Inline Assembly | Shared Object     |
|                 | mmap() memory | pragma                  | Inline Assembly | Shared Object     |
|                 | Local scope   | memset_s()              | Inline Assembly | Shared Object     |
|                 | Global scope  | memset_s()              | Inline Assembly | Shared Object     |
|                 | Heap memory   | memset_s()              | Inline Assembly | Shared Object     |
|                 | mmap() memory | memset_s()              | Inline Assembly | Shared Object     |
|                 | Local scope   | pass to do-nothing func | Inline Assembly | Shared Object     |
|                 | Global scope  | pass to do-nothing func | Inline Assembly | Shared Object     |
|                 | Heap memory   | pass to do-nothing func | Inline Assembly | Shared Object     |
|                 | mmap() memory | pass to do-nothing func | Inline Assembly | Shared Object     |
|                 | Local scope   | read/write/replace      | Inline Assembly | Shared Object     |
|                 | Global scope  | read/write/replace      | Inline Assembly | Shared Object     |
|                 | Heap memory   | read/write/replace      | Inline Assembly | Shared Object     |
|                 | mmap() memory | read/write/replace      | Inline Assembly | Shared Object     |
|                 | Local scope   | explicit_bzero          | Inline Assembly | Shared Object     |
|                 | Global scope  | explicit_bzero          | Inline Assembly | Shared Object     |
|                 | Heap memory   | explicit_bzero          | Inline Assembly | Shared Object     |
|                 | mmap() memory | explicit_bzero          | Inline Assembly | Shared Object     |

### Testing Steps

1. Write and source-control the code file
2. Add the code filename to the chart (zeroize-main/header/def-WXYZ.c
	- W == Thing number
	- X == Trick number
	- Y == Object number
	- Z == Scheme number
	- Example: Utilizing inline assembly, with the volatile keyword, that has been compiled into a shared library, on a variable of local scope would use the WXYZ number of 1235.  1 for "local scope", 2 for "volatile", 3 for "assembly", and 5 for "shared object".
3. Add the necessary recipe to the Makefile
4. Update the test file
5. Run the tests
6. Record the results in the chart above
