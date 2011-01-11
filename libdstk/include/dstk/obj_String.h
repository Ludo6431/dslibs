#ifndef _OBJ_STRING_H
#define _OBJ_STRING_H

#include "dstk/obj_AType.h"

struct String {
    struct AType _;
};
#define STRING(obj) ((struct String *)(obj))

struct cString {
    struct cAType _;
};
#define cSTRING(cl) ((struct cString *)(cl))

extern const void *String;

// ---- new functions ----

#endif

