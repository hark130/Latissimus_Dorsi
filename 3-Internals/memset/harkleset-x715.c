#include <string.h>								// memset()


void *harklexplicit(void *s, int c, size_t n)
{
	void *(*func_ptr)(void*, int, size_t) = memset;
	return func_ptr(s, c, n);
}
