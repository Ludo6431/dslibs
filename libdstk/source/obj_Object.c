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

static int Object_isclass(const void *class) {
    return class == Object || cOBJ(OList)->isclass(class);
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

static const struct cObject _Object = {
    {   // OList
        {   // Obj
            sizeof(struct Object),
            Object_ctor,
            Object_dtor,
            Object_clone,
            Object_cmp,
            Object_isclass
        },
        Object_add,
        Object_find,
        Object_drop
    }
    // setprop
    // delprop
};

const void *Object = & _Object;

// ---- new functions ----

