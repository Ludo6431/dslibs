#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "dstk/obj.h"

static void *Obj_ctor(void *_self, va_list *app) {
    OBJ(_self)->refcount = 0;
    return _self;
}

static void *Obj_dtor(void *_self) {
    return _self;
}

static void *Obj_clone(const void *_self) {
    const struct Obj *self = _self;
    struct Obj *ret = malloc(SIZE(self));
    assert(ret);

    ret->class = self->class;

    return ret;
}

static int Obj_cmp(const void *_self, const void *_b) {
    return CLASS(_self) != CLASS(_b);
}

static int Obj_isclass(const void *class) {
    return class == Obj;
}

static const struct cObj _Obj = {
    sizeof(struct Obj)  /* size */,
    Obj_ctor            /* ctor */,
    Obj_dtor            /* dtor */,
    Obj_clone           /* clone */,
    Obj_cmp             /* cmp */,
    Obj_isclass         /* isclass */
};

const void *Obj = &_Obj;

// ---- new functions ----

void *CTORV(const void *class, void *_self, ...) {
    va_list ap;
    va_start(ap, _self);
    void *ret=cOBJ(class)->ctor(_self, &ap);
    va_end(ap);

    return ret;
}

void *obj_new(const void *_class, ...) {
    const struct cObj *class = _class;
    void *new = malloc(class->size);
    assert(new);
    *(const struct cObj **)new = class;

    assert(class->ctor);
    va_list ap;
    va_start(ap, _class);
    new = class->ctor(new, &ap);
    va_end(ap);

    REFCNT(new) = 0;

    return new;
}

void obj_delete(void *_self) {
    if(_self) {
        const struct cObj *class = CLASS(_self);
        assert(class && class->dtor);

        if(!REFCNT(_self)) {
            _self = class->dtor(_self);
             free(_self);
        }
    }
}

void *obj_clone(void *_self) {
    void *new = NULL;
    if(_self) {
        const struct cObj *class = CLASS(_self);
        assert(class && class->clone);

        new = class->clone(_self);
    }
    return new;
}

int obj_cmp(const void *_self, const void *_b) {
    if(_self && _b) {
        const struct cObj *class = CLASS(_self);
        assert(class && class->cmp);

        return class->cmp(_self, _b);
    }

    return 0;
}

int obj_isclass(const void *_self, const void *_class) {
    if(_self && _class) {
        const struct cObj *class = CLASS(_self);
        assert(class && class->isclass);

        return class->isclass(_class);
    }

    return 0;
}

