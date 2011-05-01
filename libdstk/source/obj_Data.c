#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/properties.h"

#include "dstk/obj_Data.h"

#define PARENT_CLASS ((void *)&_PObj)

static void *Data_ctor(void *_self, va_list *app) {
    struct Data *self = cOBJ(PARENT_CLASS)->ctor(_self, app);

    unsigned datasize = va_arg(*app, unsigned);
    void *data = va_arg(*app, void *);

    obj_setprop(self, PROP_DATA, data, datasize);

    return self;
}

char *Data_repr(const void *_self) {
//    const struct Data *self = _self;

    return obj_getprop((void *)_self, PROP_DATA, NULL);
}

const struct cData _Data = {
    {   // PObj
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
        NULL,                   /* setp */
        NULL,                   /* getp */
        NULL                    /* delp */
    },
    Data_repr                   /* repr */
};

const void *Data = &_Data;

// ---- new functions ----

void *obj_getdata(const void *_self, unsigned *datasize) {
    if(_self && obj_isclass(_self, Data))
        return obj_getprop((void *)_self, PROP_DATA, datasize);

    return NULL;
}

char *obj_repr(const void *_self) {
    if(_self && obj_isclass(_self, Data)) {
        const struct cData *class = CLASS(_self);
        assert(class);

        INIT_CLASS(class);

        assert(class->repr);
        return class->repr(_self);
    }

    return NULL;
}

