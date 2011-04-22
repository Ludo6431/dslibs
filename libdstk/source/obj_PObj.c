#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/adata.h"

#include "dstk/obj_PObj.h"

static void *PObj_ctor(void *_self, va_list *app) {
    struct PObj *self = cOBJ(Obj)->ctor(_self, app);

    return self;
}

static void *PObj_dtor(void *_self) {
    struct PObj *self = _self;

    struct prop *pr = self->first, *next;
    while(pr) {
        next = pr->next;

        ad_del(pr->data, pr->datasize);
        free(pr);

        pr = pr->next;
    }

    return cOBJ(Obj)->dtor(_self);
}

static int PObj_setp(void *_self, unsigned key, void *data, unsigned datasize);

static void *PObj_clone(const void *_self) {
    struct PObj *new = cOBJ(Obj)->clone(_self);

    const struct PObj *self = _self;

    new->count = self->count;
    struct prop *pr = self->first;
    while(pr) {
        PObj_setp(new, pr->key, ad_get(pr->data, pr->datasize), pr->datasize);

        pr = pr->next;
    }

    return new;
}

static int PObj_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(Obj)->cmp(_self, _b);
    if(ret) return ret;

    const struct PObj *self = _self;
    const struct PObj *b = _b;

    if(self->count != b->count)
        return self->count - b->count;

    struct prop *pr1 = self->first, *pr2 = b->first;
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

static int PObj_setp(void *_self, unsigned key, void *data, unsigned datasize) {
    struct PObj *self = _self;

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

static void *PObj_getp(void *_self, unsigned key, unsigned *datasize) {
    struct PObj *self = _self;

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

static int PObj_delp(void *_self, unsigned key) {
    struct PObj *self = _self;

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

const struct cPObj _PObj = {
    {   // Obj
        sizeof(struct PObj) /* size */,
        (void *)&_Obj       /* parent */,
        PObj_ctor           /* ctor */,
        PObj_dtor           /* dtor */,
        PObj_clone          /* clone */,
        PObj_cmp            /* cmp */
    },
    PObj_setp               /* setp */,
    PObj_getp               /* getp */,
    PObj_delp               /* delp */
};

const void *PObj = &_PObj;

// ---- new functions ----

int obj_setprop(void *_self, unsigned key, void *data, unsigned datasize) {
    if(_self && obj_isclass(_self, PObj)) {
        const struct cPObj *class = CLASS(_self);
        assert(class && class->setp);

        return class->setp(_self, key, data, datasize);
    }

    return 1;
}

void *obj_getprop(void *_self, unsigned key, unsigned *datasize) {
    if(_self && obj_isclass(_self, PObj)) {
        const struct cPObj *class = CLASS(_self);
        assert(class && class->getp);

        return class->getp(_self, key, datasize);
    }

    return NULL;
}

int obj_delprop(void *_self, unsigned key) {
    if(_self && obj_isclass(_self, PObj)) {
        const struct cPObj *class = CLASS(_self);
        assert(class && class->delp);

        return class->delp(_self, key);
    }

    return 1;
}

