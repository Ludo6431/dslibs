#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

#include "dstk/obj.h"

static void *ctor(const void *class, va_list *app) {
    struct Obj *new = malloc(C_SIZE(class));
    assert(new);

    new->class = class;
    new->refcount = 0;

    return new;
}

static void dtor(void *_self) {
    assert(_self);
    free(_self);
}

static void *clone(const void *_self) {
    const struct Obj *self = _self;
    struct Obj *new = malloc(O_SIZE(self));
    assert(new);

    new->class = self->class;
    new->refcount = 0;

    return new;
}

static int cmp(const void *_self, const void *_b) {
    return CLASS(_self) != CLASS(_b);
}

const struct cObj _Obj = {
    sizeof(struct Obj)      /* size */,
    sizeof(struct cObj)     /* csize */,
    CFL_DEFAULTS | CFL_INIT /* flags */,
    NULL                    /* parent */,
    ctor                    /* ctor */,
    dtor                    /* dtor */,
    clone                   /* clone */,
    cmp                     /* cmp */
};

const void *Obj = &_Obj;

// ---- new functions ----

void _fill_handlers(void *class, unsigned foff) {
    void *cl, *data;

    #define _FIELD(cl, off) (*(void **)(&(((char *)(cl))[(off)])))
    #define IS_IN_CLASS(cl, off) ((off) + sizeof(void *) <= C_CSIZE(cl))

    while(1) {
        while(1) {
            if(!class)
                return;

            if(C_FLAGS(class)&CFL_INIT || !IS_IN_CLASS(class, foff))
                return;

            if(!_FIELD(class, foff))
                break;

            class = C_PARENT(class);
        }

        assert(class && !(C_FLAGS(class)&CFL_INIT) && IS_IN_CLASS(class, foff) && !_FIELD(class, foff));

        cl = class;
        while(!_FIELD(cl, foff)) {
            cl = C_PARENT(cl);
            assert(cl);
            assert(IS_IN_CLASS(cl, foff));
        }

        data = _FIELD(cl, foff);

        cl = class;
        while(!_FIELD(cl, foff)) {
            _FIELD(cl, foff) = data;
            cl = C_PARENT(cl);
        }

        class = C_PARENT(cl);
    }

    #undef IS_IN_CLASS
    #undef _FIELD
}

void _init_handlers(void *_class) {
    struct cObj *class = _class;
    assert(class);

    if(C_FLAGS(class)&CFL_INIT)
        return;

    unsigned off;

    for(off=(void *)&class->ctor - (void *)class; off<C_CSIZE(class); off+=sizeof(void *))
        _fill_handlers(class, off);

    for(;class; class = C_PARENT(class))
        C_FLAGS(class) |= CFL_INIT;
}

inline void *CTORV(void *(*ctor_f)(const void *, va_list *), const void *class, ...) {
    void *new;

    va_list ap;
    va_start(ap, class);
    new = ctor_f(class, &ap);
    va_end(ap);

    return new;
}

void *obj_new(const void *_class, ...) {
    const struct cObj *class = _class;
    assert(class);
    struct Obj *new;

    INIT_CLASS(class);

    assert(class->ctor);
    va_list ap;
    va_start(ap, _class);
    new = class->ctor(class, &ap);
    va_end(ap);

    return new;
}

void obj_delete(void *_self) {
    if(!_self || O_REFCNT(_self))
        return;

    const struct cObj *class = CLASS(_self);
    assert(class);

    assert(class->dtor);
    class->dtor(_self);
}

void *obj_clone(void *_self) {
    assert(_self);

    const struct cObj *class = CLASS(_self);
    assert(class);

    assert(class->clone);
    return class->clone(_self);
}

int obj_cmp(const void *_self, const void *_b) {
    assert(_self && _b);

    const struct cObj *class = CLASS(_self);
    assert(class);

    assert(class->cmp);
    return class->cmp(_self, _b);
}

int obj_isclass(const void *_self, const void *_class) {
    if(_self && _class) {
        const struct cObj *class = CLASS(_self);

        while(class) {
            if(class == _class)
                return 1;

            class = class->parent;
        }
    }

    return 0;
}

