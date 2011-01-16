#ifndef _TOOLS_H
#define _TOOLS_H

#include <string.h> // for bzero

#include "list.h"
#include "dft.h"

#define NEW(type) (type *)malloc(sizeof(type))
#define RESET(d, type) bzero((type *)(d), sizeof(type))
#define NEWZ(type) (type *)calloc(1, sizeof(type))

#ifndef MIN
#   define MIN(a, b) ((a)>(b) ? (b) : (a))
#endif

#ifndef MAX
#   define MAX(a, b) ((a)>(b) ? (a) : (b))
#endif

#define ALIGN(n, b) ((n + b - 1) & ~(b - 1))

void mexit(int ret, char *format, ...);
char *add_suffix(char *base, char *suffix);


#define SGLYPH(el)  ((sDFT_GLYPH *)(((sLEl *)(el))->data))
#define SRANGE(el)  ((sDFT_RANGE *)(((sLEl *)(el))->data))

void add_range(sList *ranges, unsigned start, unsigned end);

#endif

