#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/obj_AType.h"

#define SZMAX (sizeof(unsigned))

static void *AType_ctor(void *_self, va_list *app) {
    struct AType *self = cOBJ(Obj)->ctor(_self, app);

    self->datasize = va_arg(*app, unsigned);
    if(self->datasize <= SZMAX)
        self->data = *va_arg(*app, unsigned *);
    else {
        self->data = (unsigned)malloc(self->datasize);
        assert(self->data);
        memcpy((void *)self->data, (const void *)va_arg(*app, unsigned *), self->datasize);
    }

    return self;
}

static void *AType_dtor(void *_self) {
    struct AType *self = _self;

    if(self->datasize > SZMAX && self->data)
        free((void *)self->data);

    self->datasize = 0;

    return cOBJ(Obj)->dtor(_self);
}

static void *AType_clone(const void *_self) {
    const struct AType *self = _self;
    struct AType *new = cOBJ(Obj)->clone(_self);

    new->datasize = self->datasize;
    if(self->datasize <= SZMAX) {
        new->data = self->data;
    }
    else {
        new->data = (unsigned)malloc(self->datasize);
        assert(new->data);
        memcpy((void *)new->data, (void *)self->data, self->datasize);
    }

    return new;
}

static int AType_cmp(const void *_self, const void *_b) {
    const struct AType *self = _self;
    const struct AType *b = _b;

    int ret = cOBJ(Obj)->cmp(_self, _b);
    if(ret) return ret;

    if(self->datasize != b->datasize)
        return self->datasize - b->datasize;

    if(self->datasize <= SZMAX && self->data != b->data)
        return self->data - b->data;

    return memcmp((const void *)self->data, (const void *)b->data, self->datasize);
}

static int AType_isclass(const void *class) {
    return class == AType || cOBJ(Obj)->isclass(class);
}

static int AType_repr(const void *_self, char *s, unsigned l) {
    const struct AType *self = _self;
    int i;
    unsigned dsz;

    dsz = MIN(l, self->datasize);

    if(self->datasize <= SZMAX) {
        for(i=0; i<dsz; i++)
            s[i] = ((char *)&self->data)[i];
    }
    else
        memcpy((void *)s, (void *)self->data, dsz);

    return dsz != self->datasize;
}

static const struct cAType _AType = {
    {   // Obj
        sizeof(struct AType)    /* size */,
        AType_ctor              /* ctor */,
        AType_dtor              /* dtor */,
        AType_clone             /* clone */,
        AType_cmp               /* cmp */,
        AType_isclass           /* isclass */
    },
    AType_repr  /* repr */
};

const void *AType = &_AType;

// ---- new functions ----

char *obj_repr(const void *_self) {
    static char tmp[256];

    if(_self && obj_isclass(_self, AType)) {
        const struct cAType *class = CLASS(_self);
        assert(class && class->repr);

        class->repr(_self, tmp, sizeof(tmp));

        return tmp;
    }

    return NULL;
}

