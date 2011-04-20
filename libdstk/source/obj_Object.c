#include <stdlib.h>
#include <assert.h>

#include "dstk/obj_Object.h"

static void *Object_ctor(void *_self, va_list *app) {
    struct Object *self = cOBJ(OList)->ctor(_self, app);

    // set properties system

    return self;
}

static void *Object_dtor(void *_self) {
    return cOBJ(OList)->dtor(_self);
}

static void *Object_clone(const void *_self) {
    return cOBJ(OList)->clone(_self);
}

static int Object_cmp(const void *_self, const void *_b) {
    return cOBJ(OList)->cmp(_self, _b);
}

static void *Object_add(void *_self, void *element) {
    return cOLIST(OList)->add(_self, element);
}

static void *Object_find(void *_self, void *element) {
    return cOLIST(OList)->find(_self, element);
}

static void *Object_drop(void *_self, void *element) {
    return cOLIST(OList)->drop(_self, element);
}

const struct cObject _Object = {
    {   // OList
        {   // Obj
            sizeof(struct Object)   /* size */,
            (void *)&_OList         /* parent */,
            Object_ctor             /* ctor */,
            Object_dtor             /* dtor */,
            Object_clone            /* clone */,
            Object_cmp              /* cmp */
        },
        Object_add                  /* add */,
        Object_find                 /* find */,
        Object_drop                 /* drop */
    }
    // setprop
    // getprop
    // delprop
};

const void *Object = & _Object;

// ---- new functions ----

