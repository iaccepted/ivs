#include <stdlib.h>
#include <string.h>

#include "memory.h"

void *xzalloc(size_t size)
{
    void *p = NULL;

    if (size == 0) {
        return p;
    }

    p = malloc(size);
    if (p == NULL) {
        exit(1);
    }

    memset(p, 0, size);
    return p;
}

void xfree(void *p)
{
    if (p != NULL) {
        free(p);
    }
}

