#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "common.h"

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

