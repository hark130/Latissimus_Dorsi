/*
 * ch03-memaddr.c --- Show address of code, data and stack sections, as well as BSS and dynamic memory.
 * This program prints the locations of the two functions main() and afunc() (lines 22–23). 
 * It then shows how the stack grows downward, letting afunc() (lines 51–63) print the address of successive 
 *  instantiations of its local variable stack_var. (stack_var is purposely declared auto, to emphasize that 
 *  it's on the stack.) It then shows the location of memory allocated by alloca() (lines 28–32). 
 * Finally it prints the locations of data and BSS variables (lines 34–38), and then of memory allocated 
 *  directly through sbrk() (lines 40–48). Refernce: http://www.phptr.com/articles/article.asp?p=173438&seqNum=2
 */

#include <stdio.h>
#include <malloc.h>     /* for definition of ptrdiff_t on GLIBC */
#include <unistd.h>
#include <alloca.h>     /* for demonstration only */

extern void afunc(void);    /* a function for showing stack growth */

int bss_var;            /* auto init to 0, should be in BSS */
int data_var = 42;      /* init to nonzero, should be data */

int
main(int argc, char **argv) /* arguments aren't used */
{
    char *p, *b, *nb;

    printf("Text Locations:\n");
    printf("\tAddress of main: %p\n", main);
    printf("\tAddress of afunc: %p\n", afunc);

    printf("Stack Locations:\n");
    afunc();

    p = (char *) alloca(32);
    if (p != NULL) {
        printf("\tStart of alloca()'ed array: %p\n", p);
        printf("\tEnd of alloca()'ed array: %p\n", p + 31);
    }

    printf("Data Locations:\n");
    printf("\tAddress of data_var: %p\n", & data_var);

    printf("BSS Locations:\n");
    printf("\tAddress of bss_var: %p\n", & bss_var);

    b = sbrk((ptrdiff_t) 32);   /* grow address space */
    nb = sbrk((ptrdiff_t) 0);
    printf("Heap Locations:\n");
    printf("\tInitial end of heap: %p\n", b);
    printf("\tNew end of heap: %p\n", nb);

    b = sbrk((ptrdiff_t) -16);  /* shrink it */
    nb = sbrk((ptrdiff_t) 0);
    printf("\tFinal end of heap: %p\n", nb);

    /* wait */
    getchar();
}

void
afunc(void)
{
    static int level = 0;       /* recursion level */
    auto int stack_var;        /* automatic variable, on stack */

    if (++level == 3)           /* avoid infinite recursion */
        return;

    printf("\tStack level %d: address of stack_var: %p\n",
            level, & stack_var);
    afunc();                    /* recursive call */
}