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
    struct Obj *ret = malloc(O_SIZE(self));
    assert(ret);

    ret->class = self->class;
    ret->refcount = 0;

    return ret;
}

static int Obj_cmp(const void *_self, const void *_b) {
    return CLASS(_self) != CLASS(_b);
}

const struct cObj _Obj = {
    sizeof(struct Obj)      /* size */,
    sizeof(struct cObj)     /* csize */,
    CFL_DEFAULTS | CFL_INIT /* flags */,
    NULL                    /* parent */,
    Obj_ctor                /* ctor */,
    Obj_dtor                /* dtor */,
    Obj_clone               /* clone */,
    Obj_cmp                 /* cmp */
};

const void *Obj = &_Obj;

// ---- new functions ----

void _fill_handlers(void *class, unsigned foff) {
    void *cl, *data;

    #define _FIELD(cl, off) (*(void **)(&(((char *)(cl))[(off)])))
    #define IS_IN_CLASS(cl, off) ((off) + sizeof(void *) <= C_CSIZE(cl))

//printf("foff=%d\n", foff);
//printf("class=%p\n", class);

    while(1) {
        while(1) {
            if(!class)
                return;

            if(C_FLAGS(class)&CFL_INIT || !IS_IN_CLASS(class, foff))
                return;

            if(!_FIELD(class, foff))
                break;

            class = C_PARENT(class);
//printf("class=%p\n", class);
        }

        assert(class && !(C_FLAGS(class)&CFL_INIT) && IS_IN_CLASS(class, foff) && !_FIELD(class, foff));

        cl = class;
        while(!_FIELD(cl, foff)) {
            cl = C_PARENT(cl);
//printf("cl=%p(%s)\n", cl, cl==Obj?"Obj":"other");
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

    for(off=(void *)&class->ctor - (void *)class; off<C_CSIZE(class); off+=sizeof(void *)) {
//printf("--class=%p|off=%d--\n", class, off);
        _fill_handlers(class, off);
    }

    for(;class; class = C_PARENT(class))
        C_FLAGS(class) |= CFL_INIT;
}

void *CTORV(const void *class, void *_self, ...) {
    va_list ap;
    va_start(ap, _self);
    void *ret=cOBJ(class)->ctor(_self, &ap);
    va_end(ap);

    return ret;
}

void *obj_new(const void *_class, ...) {
    const struct cObj *class = _class;
    assert(class);
    void *new = malloc(class->size);
    assert(new);
    CLASS(new) = class;

    INIT_CLASS(class);

    assert(class->ctor);
    va_list ap;
    va_start(ap, _class);
    new = class->ctor(new, &ap);
    va_end(ap);

    return new;
}

void obj_delete(void *_self) {
    if(!_self)
        return;

    const struct cObj *class = CLASS(_self);
    assert(class);

    INIT_CLASS(class);

    assert(class->dtor);
    if(!O_REFCNT(_self)) {
        _self = class->dtor(_self);
         free(_self);
    }
}

void *obj_clone(void *_self) {
    void *new = NULL;
    if(_self) {
        const struct cObj *class = CLASS(_self);
        assert(class);

        INIT_CLASS(class);

        assert(class->clone);
        new = class->clone(_self);
    }
    return new;
}

int obj_cmp(const void *_self, const void *_b) {
    if(_self && _b) {
        const struct cObj *class = CLASS(_self);
        assert(class);

        INIT_CLASS(class);

        assert(class->cmp);
        return class->cmp(_self, _b);
    }

    return 0;
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

