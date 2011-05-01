#include <stdlib.h>
#include <assert.h>

#include "dstk/obj_Object.h"

#define PARENT_CLASS ((void *)&_OList)

static void *Object_ctor(void *_self, va_list *app) {
    struct Object *self = cOBJ(PARENT_CLASS)->ctor(_self, app);

    // set properties system

    return self;
}

static void *Object_dtor(void *_self) {
    return cOBJ(PARENT_CLASS)->dtor(_self);
}

static void *Object_clone(const void *_self) {
    return cOBJ(PARENT_CLASS)->clone(_self);
}

static int Object_cmp(const void *_self, const void *_b) {
    return cOBJ(PARENT_CLASS)->cmp(_self, _b);
}

const struct cObject _Object = {
    {   // OList
        {   // Obj
            sizeof(struct Object)   /* size */,
            sizeof(struct cObject)  /* csize */,
            CFL_DEFAULTS            /* flags */,
            PARENT_CLASS            /* parent */,
            Object_ctor             /* ctor */,
            Object_dtor             /* dtor */,
            Object_clone            /* clone */,
            Object_cmp              /* cmp */
        },
        NULL                        /* add */,
        NULL                        /* find */,
        NULL                        /* drop */
    }
};

const void *Object = & _Object;

// ---- new functions ----

