#ifndef _OBJ_STRING_H
#define _OBJ_STRING_H

#include "dstk/obj_Data.h"

struct String {
    struct Data _;
};
#define STRING(obj) ((struct String *)(obj))

struct cString {
    struct cData _;
};
#define cSTRING(cl) ((struct cString *)(cl))

extern const struct cString _String;
extern const void *String;

// ---- new functions ----

#endif

