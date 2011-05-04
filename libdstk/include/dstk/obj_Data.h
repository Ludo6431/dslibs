#ifndef _OBJ_DATA_H
#define _OBJ_DATA_H

#include "dstk/obj_AObj.h"

struct Data {
    struct AObj _;
};
#define DATA(obj) ((struct Data *)(obj))

struct cData {
    struct cAObj _;

    char *  (*repr) (const void *self);
};
#define cDATA(cl) ((struct cData *)(cl))

extern const struct cData _Data;
extern const void *Data;

// ---- new functions ----

void *  obj_getdata (const void *self, unsigned *datasize);
char *  obj_repr    (const void *self);

#endif

