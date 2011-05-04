#ifndef _OBJ_AOBJ_H
#define _OBJ_AOBJ_H

#include "dstk/adata.h"

#include "dstk/obj.h"

struct prop {
    unsigned key;

    adata_t data;
    unsigned datasize;

    struct prop *next;
};

struct AObj {
    struct Obj _;

    unsigned count;
    struct prop *first;
    struct prop *last;
};
#define AOBJ(obj) ((struct AObj *)(obj))

struct cAObj {
    struct cObj _;

    int     (*setp)    (void *self, unsigned key, void *data, unsigned datasize);
    void *  (*getp)    (void *self, unsigned key, unsigned *destsize);
    int     (*delp)    (void *self, unsigned key);
};
#define cAOBJ(cl) ((struct cAObj *)(cl))

extern const struct cAObj _AObj;
extern const void *AObj;

// ---- new functions ----

int     obj_setprop (void *self, unsigned key, void *data, unsigned datasize);
void *  obj_getprop (void *self, unsigned key, unsigned *datasize);
int     obj_delprop (void *self, unsigned key);

#endif

