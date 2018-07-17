#include <string.h>								// memset()


void *harkleset(void *s, int c, size_t n)
{
	return memset(s, c, n);
}
