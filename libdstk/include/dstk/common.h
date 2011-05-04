#ifndef _COMMON_H
#define _COMMON_H

#ifndef MIN
#   define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#   define MAX(a, b) ((a) < (b) ? (b) : (a))
#endif

#ifndef BIT
#   define BIT(b) (1<<(b))
#endif

void dstk_assert(const char *file, int line, const char *func, const char *cond);
#undef assert
#define assert(c) ((c)?(void)0:dstk_assert(__FILE__, __LINE__, __func__, #c))

#if 0
void dstk_free(void *p, const char *func);
#define free(p) dstk_free(p, __func__)

void *dstk_malloc(unsigned sz, const char *func);
#define malloc(sz) dstk_malloc(sz, __func__)

void *dstk_calloc(unsigned n, unsigned sz, const char *func);
#define calloc(n, sz) dstk_calloc(n, sz, __func__)
#endif

#endif

