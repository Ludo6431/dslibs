#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/obj_AObj.h"

#define PARENT_CLASS ((void *)&_Obj)

static void *AObj_ctor(const void *class, va_list *app) {
    struct AObj *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    new->count = 0;

    return new;
}

static void AObj_dtor(void *_self) {
    struct AObj *self = _self;

    struct prop *pr = self->first, *next;
    while(pr && self->count>0) {
        next = pr->next;

        ad_del(pr->data, pr->datasize);
        free(pr);

        self->count--;
        pr = pr->next;
    }
    assert(!self->count);

    cOBJ(PARENT_CLASS)->dtor(_self);
}

static int AObj_setp(void *_self, unsigned key, void *data, unsigned datasize);

static void *AObj_clone(const void *_self) {
    struct AObj *new = cOBJ(PARENT_CLASS)->clone(_self);

    const struct AObj *self = _self;

    new->count = 0;

    struct prop *pr = self->first;
    while(pr) {
        AObj_setp(new, pr->key, ad_get(pr->data, pr->datasize), pr->datasize);

        pr = pr->next;
    }

    return new;
}

static int AObj_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(PARENT_CLASS)->cmp(_self, _b);
    if(ret) return ret;

    const struct AObj *self = _self;
    const struct AObj *b = _b;

    if(self->count != b->count)
        return self->count - b->count;

    struct prop *pr1 = self->first, *pr2 = b->first;    // TODO : the order doesn't matter
    while(pr1) {
        if(pr1->datasize != pr2->datasize)
            return pr1->datasize - pr2->datasize;

        int ret = memcmp(ad_get(pr1->data, pr1->datasize), ad_get(pr2->data, pr2->datasize), pr1->datasize);
        if(ret)
            return ret;

        pr1 = pr1->next;
        pr2 = pr2->next;
    }

    return 0;
}

static int AObj_setp(void *_self, unsigned key, void *data, unsigned datasize) {
    struct AObj *self = _self;

    struct prop *pr = NULL;
    if(self->count) {
        pr = self->first;
        while(pr) {
            if(pr->key == key)
                break;

            pr = pr->next;
        }

        if(pr)
            ad_del(pr->data, pr->datasize);
        else {
            pr = self->last->next = calloc(1, sizeof(struct prop));
            if(!pr)
                return 1;
            self->last = self->last->next;
            self->count++;
        }
    }
    else {
        self->first = self->last = pr = calloc(1, sizeof(struct prop));
        if(!pr)
            return 1;
        self->count++;
    }

    pr->key = key;
    pr->data = ad_new(data, datasize);
    pr->datasize = datasize;

    return 0;
}

static void *AObj_getp(void *_self, unsigned key, unsigned *datasize) {
    struct AObj *self = _self;

    if(!self->count)
        return NULL;

    struct prop *pr = self->first;
    while(pr) {
        if(pr->key == key)
            break;

        pr = pr->next;
    }

    if(!pr)
        return NULL;

    if(datasize)
        *datasize = pr->datasize;
    return ad_get(pr->data, pr->datasize);
}

static int AObj_delp(void *_self, unsigned key) {
    struct AObj *self = _self;

    if(!self->count)
        return 1;

    struct prop *pr = self->first, *prev = NULL;
    while(pr) {
        if(pr->key == key)
            break;

        prev = pr;
        pr = pr->next;
    }

    if(!pr)
        return 1;

    if(pr == self->first)
        self->first = self->first->next;
    if(pr == self->last)
        self->last = prev;
    if(prev)
        prev->next = pr->next;

    ad_del(pr->data, pr->datasize);
    free(pr);

    self->count--;

    return 0;
}

const struct cAObj _AObj = {
    {   // Obj
        sizeof(struct AObj) /* size */,
        sizeof(struct cAObj)/* csize */,
        CFL_DEFAULTS        /* flags */,
        PARENT_CLASS        /* parent */,
        AObj_ctor           /* ctor */,
        AObj_dtor           /* dtor */,
        AObj_clone          /* clone */,
        AObj_cmp            /* cmp */
    },
    AObj_setp               /* setp */,
    AObj_getp               /* getp */,
    AObj_delp               /* delp */
};

const void *AObj = &_AObj;

// ---- new functions ----

int obj_setprop(void *_self, unsigned key, void *data, unsigned datasize) {
    assert(_self && obj_isclass(_self, AObj));

    const struct cAObj *class = CLASS(_self);
    assert(class);

    INIT_CLASS(class);

    assert(class->setp);
    return class->setp(_self, key, data, datasize);
}

void *obj_getprop(void *_self, unsigned key, unsigned *datasize) {
    assert(_self && obj_isclass(_self, AObj));

    const struct cAObj *class = CLASS(_self);
    assert(class);

    INIT_CLASS(class);

    assert(class->getp);
    return class->getp(_self, key, datasize);
}

int obj_delprop(void *_self, unsigned key) {
    assert(_self && obj_isclass(_self, AObj));

    const struct cAObj *class = CLASS(_self);
    assert(class);

    INIT_CLASS(class);

    assert(class->delp);
    return class->delp(_self, key);
}

