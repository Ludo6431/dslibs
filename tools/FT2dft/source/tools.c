#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include "FT2dft.h"

#include "tools.h"

// little exit helper
void mexit(int ret, char *format, ...) {
    if(format) {
        va_list ap;
        va_start(ap, format);
        vfprintf(stderr, format, ap);
        fprintf(stderr, "\n");
        va_end(ap);
    }
    exit(ret);
}

char *add_suffix(char *base, char *suffix) {
    #define LENMAX 1024
    static char new[LENMAX];

    if(strlen(base)+strlen(suffix)+1>LENMAX) return NULL;

    strcpy(new, base);
    strcat(new, suffix);

    return new;
}


void add_range(sList *ranges, unsigned start, unsigned end) {
    sDFT_RANGE *range = NEWZ(sDFT_RANGE);
    assert(range);

    range->map_offset = ranges->tail?(SRANGE(ranges->tail)->map_offset + SRANGE(ranges->tail)->last_car - SRANGE(ranges->tail)->first_car + 1):0;
    range->first_car = start;
    range->last_car = end;

    L_add_tail(ranges, range);
}

