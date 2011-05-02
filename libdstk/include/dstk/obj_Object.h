#ifndef _OBJ_OBJECT_H
#define _OBJ_OBJECT_H

#include "dstk/obj_OList.h"

struct Object {
    struct OList _;
};
#define OBJECT(obj) ((struct Object *)(obj))

struct cObject {
    struct cOList _;
};
#define cOBJECT(cl) ((struct cObject *)(cl))

extern const struct cObject _Object;
extern const void *Object;

// ---- new functions ----

#endif

