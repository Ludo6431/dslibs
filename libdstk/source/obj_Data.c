#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/adata.h"

#include "dstk/obj_Data.h"

#define SZMAX (sizeof(unsigned))

static void *Data_ctor(void *_self, va_list *app) {
    struct Data *self = cOBJ(Obj)->ctor(_self, app);

    self->datasize = va_arg(*app, unsigned);
    self->data = ad_new(va_arg(*app, void *), self->datasize);

    return self;
}

static void *Data_dtor(void *_self) {
    struct Data *self = _self;

    ad_del(self->data, self->datasize);
    self->data = self->datasize = 0;

    return cOBJ(Obj)->dtor(_self);
}

static void *Data_clone(const void *_self) {
    const struct Data *self = _self;
    struct Data *new = cOBJ(Obj)->clone(_self);

    new->data = ad_new(ad_get(self->data, self->datasize), self->datasize);
    new->datasize = self->datasize;

    return new;
}

static int Data_cmp(const void *_self, const void *_b) {
    const struct Data *self = _self;
    const struct Data *b = _b;

    int ret = cOBJ(Obj)->cmp(_self, _b);
    if(ret) return ret;

    if(self->datasize != b->datasize)
        return self->datasize - b->datasize;

    if(self->datasize <= SZMAX && self->data != b->data)
        return self->data - b->data;

    return memcmp((const void *)self->data, (const void *)b->data, self->datasize);
}

static int Data_repr(const void *_self, char *s, unsigned l) {  // TODO : useless when the propoerty system will be used
    const struct Data *self = _self;

    memcpy((void *)s, ad_get(self->data, self->datasize), MIN(l, self->datasize));

    return l < self->datasize;
}

const struct cData _Data = {
    {   // Obj
        sizeof(struct Data) /* size */,
        (void *)&_Obj       /* parent */,
        Data_ctor           /* ctor */,
        Data_dtor           /* dtor */,
        Data_clone          /* clone */,
        Data_cmp            /* cmp */
    },
    Data_repr               /* repr */
};

const void *Data = &_Data;

// ---- new functions ----

char *obj_repr(const void *_self) { // TODO : use property system to know the size of the data
    static char tmp[256];

    if(_self && obj_isclass(_self, Data)) {
        const struct cData *class = CLASS(_self);
        assert(class && class->repr);

        class->repr(_self, tmp, sizeof(tmp));

        return tmp;
    }

    return NULL;
}

