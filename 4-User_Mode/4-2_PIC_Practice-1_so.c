/*
    FILE: 4-2_PIC_Practice-1.c
    COMPILE: gcc
    PURPOSE: Write Position Independent Code
    NOTE: This code specifically replicates code found on https://eli.thegreenplace.net/2011/11/03/position-independent-code-pic-in-shared-libraries/
        but modified ever so slightly.
 */

int globInt = 42;

int func_4_2_1(int a, int b)
{
    return globInt + a + b;
}

// Temporarily added this to test the Makefile
// int main(void)
// {
//     return func4_2_1(1, 2);
// }
