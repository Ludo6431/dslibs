#ifndef _OBJ_POBJ_H
#define _OBJ_POBJ_H

#include "dstk/obj.h"

struct prop {
    unsigned key;

    unsigned data;
    unsigned datasize;

    struct prop *next;
};

struct PObj {
    struct Obj _;

    unsigned count;
    struct prop *first;
    struct prop *last;
};
#define POBJ(obj) ((struct PObj *)(obj))

struct cPObj {
    struct cObj _;

    int     (*setp)    (void *self, unsigned key, void *data, unsigned datasize);
    void *  (*getp)    (void *self, unsigned key, unsigned *destsize);
    int     (*delp)    (void *self, unsigned key);
};
#define cPOBJ(cl) ((struct cPObj *)(cl))

extern const struct cPObj _PObj;
extern const void *PObj;

// ---- new functions ----

int     obj_setprop (void *self, unsigned key, void *data, unsigned datasize);
void *  obj_getprop (void *self, unsigned key, unsigned *datasize);
int     obj_delprop (void *self, unsigned key);

#endif

