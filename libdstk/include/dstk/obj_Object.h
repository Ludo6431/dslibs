#ifndef _OBJ_OBJECT_H
#define _OBJ_OBJECT_H

#include "dstk/obj_OList.h"

struct Object {
    struct OList _;

//    struct Dico *properties;
    // add signals ?
};
#define OBJECT(obj) ((struct Object *)(obj))

struct cObject {
    struct cOList _;

//    void *  setprop (void *self, 
//    void *  getprop (void *self, 
//    void *  delprop (void *self, 
};
#define cOBJECT(cl) ((struct cObject *)(cl))

extern const void *Object;

// ---- new functions ----

#endif

