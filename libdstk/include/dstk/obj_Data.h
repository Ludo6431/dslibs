#ifndef _OBJ_DATA_H
#define _OBJ_DATA_H

#include "dstk/obj.h"

struct Data {
    struct Obj _;

    unsigned datasize;
    unsigned data;
};
#define DATA(obj) ((struct Data *)(obj))

struct cData {
    struct cObj _;

    int     (*repr) (const void *self, char *s, unsigned l);
};
#define cDATA(cl) ((struct cData *)(cl))

extern const struct cData _Data;
extern const void *Data;

// ---- new functions ----

char *obj_repr(const void *self);

#endif

