#include <stdlib.h>
#include <stdio.h>

#include "dstk/common.h"

void dstk_assert(const char *file, int line, const char *func, const char *cond) {
    printf("\n__Assertion failed__\nat %s:%d in %s with \"%s\"\n", file, line, func, cond);
    while(1);
}

void dstk_free(void *p, const char *func) {
    printf("[%s]free(%p)", func, p);
    if(!p) {
        while(1);
    }

#undef free
    free(p);
}

void *dstk_malloc(unsigned sz, const char *func) {
    printf("[%s]malloc(%d)=", func, sz);
#undef malloc
    void *p = malloc(sz);

    printf("%p", p);

    return p;
}


void *dstk_calloc(unsigned n, unsigned sz, const char *func) {
    printf("[%s]calloc(%d, %d)=", func, n, sz);

#undef calloc
    void *p = calloc(n, sz);

    printf("%p", p);

    return p;
}

