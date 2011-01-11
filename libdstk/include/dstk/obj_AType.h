#ifndef _OBJ_ATYPE_H
#define _OBJ_ATYPE_H

#include "dstk/obj.h"

struct AType {
    struct Obj _;

    unsigned datasize;
    unsigned data;
};
#define ATYPE(obj) ((struct AType *)(obj))

struct cAType {
    struct cObj _;

    int     (*repr) (const void *self, char *s, unsigned l);
};
#define cATYPE(cl) ((struct cAType *)(cl))

extern const void *AType;

// ---- new functions ----

char *obj_repr(const void *self);

#endif

