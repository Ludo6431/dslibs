#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/properties.h"

#include "dstk/obj_Data.h"

#define PARENT_CLASS ((void *)&_Object)

static void *Data_ctor(const void *class, va_list *app) {
    struct Data *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    unsigned datasize = va_arg(*app, unsigned);
    void *data = va_arg(*app, void *);

    obj_setprop(new, PROP_DATA, data, datasize);

    return new;
}

char *Data_repr(const void *_self) {
//    const struct Data *self = _self;

    return obj_getprop((void *)_self, PROP_DATA, NULL);
}

const struct cData _Data = {
    {   // Object
        {   // Obj
            sizeof(struct Data) /* size */,
            sizeof(struct cData)/* csize */,
            CFL_DEFAULTS        /* flags */,
            PARENT_CLASS        /* parent */,
            Data_ctor           /* ctor */,
            NULL                /* dtor */,
            NULL                /* clone */,
            NULL                /* cmp */
        },
        NULL                    /* sigemit */
    },
    Data_repr                   /* repr */
};

const void *Data = &_Data;

// ---- new functions ----

void *obj_getdata(const void *_self, unsigned *datasize) {
    assert(_self && obj_isclass(_self, Data));

    return obj_getprop((void *)_self, PROP_DATA, datasize);
}

char *obj_repr(const void *_self) {
    assert(_self && obj_isclass(_self, Data));

    const struct cData *class = CLASS(_self);
    assert(class);

    assert(class->repr);
    return class->repr(_self);
}

