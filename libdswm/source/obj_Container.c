#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include <dstk.h>

#include "dswm/obj_Container.h"

#define PARENT_CLASS ((void *)&_Widget)

static void *Container_add(void *_self, void *element);

static void *Container_ctor(const void *class, va_list *app) {
    struct Container *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    new->count = 0;

    void *data;
    while((data = va_arg(*app, void *)))
        Container_add(new, data);

    return new;
}

static void Container_dtor(void *_self) {
    struct Container *self = _self;

    struct list *tmp = self->first, *next;
    while(tmp && self->count>0) {
        next = tmp->next;

        O_REFCNT(tmp->data)--;
        obj_delete(tmp->data);
        slice_delete(struct list, tmp);

        self->count--;
        tmp = next;
    }
    assert(!self->count);

    cOBJ(PARENT_CLASS)->dtor(_self);
}

static void *Container_clone(const void *_self) {
    struct Container *new = cOBJ(PARENT_CLASS)->clone(_self);

    const struct Container *self = _self;

    new->count = 0;

    struct list *tmp = self->first;
    while(tmp) {
        Container_add(new, obj_clone(tmp->data));

        tmp = tmp->next;
    }

    return new;
}

static int Container_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(PARENT_CLASS)->cmp(_self, _b);
    if(ret) return ret;

    const struct Container *self = _self;
    const struct Container *b = _b;

    if(self->count != b->count)
        return self->count - b->count;

    struct list *tmp1 = self->first, *tmp2 = b->first;
    while(tmp1) {
        ret = cOBJ(Obj)->cmp(tmp1->data, tmp2->data);   // just verify it's the same class type ?
        if(ret) return ret;

        tmp1 = tmp1->next;
        tmp2 = tmp2->next;
    }

    return 0;
}

static void *Container_add(void *_self, void *element) {
    struct Container *self = _self;

    assert(element && obj_isclass(element, Obj));

    O_REFCNT(element)++;

    struct list *new = slice_new(struct list);
    assert(new);
    new->data = element;
    new->next = NULL;

    if(self->count) {
        assert(self->last);

        self->last->next = new;
        self->last = self->last->next;
    }
    else
        self->first = self->last = new;

    self->count++;

    return element;
}

static void *Container_find(void *_self, void *element) {
    struct Container *self = _self;

    if(self->count) {
        struct list *tmp = self->first;
        while(tmp) {
            if(tmp->data == element)
                return element;

            tmp = tmp->next;
        }
    }

    return NULL;
}

static void *Container_drop(void *_self, void *element) {
    struct Container *self = _self;

    struct list *curr = self->first, *prev = NULL;
    while(curr) {
        if(curr->data == element)
            break;

        prev = curr;
        curr = curr->next;
    }

    if(!curr)
        return NULL;

    if(curr == self->first)
        self->first = self->first->next;
    if(curr == self->last)
        self->last = prev;
    if(prev)
        prev->next = curr->next;
    slice_delete(struct list, curr);
    self->count--;

    O_REFCNT(element)--;
    return element;
}

const struct cContainer _Container = {
    {   // Widget
        {   // Object
            {   // Obj
                sizeof(struct Container)    /* size */,
                sizeof(struct cContainer)   /* csize */,
                CFL_DEFAULTS                /* flags */,
                PARENT_CLASS                /* parent */,
                Container_ctor              /* ctor */,
                Container_dtor              /* dtor */,
                Container_clone             /* clone */,
                Container_cmp               /* cmp */
            },
            NULL                            /* sigemit */
        },
        // TODO
    },
    Container_add                           /* add */,
    Container_find                          /* find */,
    Container_drop                          /* drop */
};

const void *Container = &_Container;

// ---- new functions ----

void *obj_add(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, Container));

    const struct cContainer *class = CLASS(_self);
    assert(class);

    assert(class->add);
    return class->add(_self, _element);
}

void *obj_find(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, Container));

    const struct cContainer *class = CLASS(_self);
    assert(class);

    assert(class->find);
    return class->find(_self, _element);
}

void *obj_drop(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, Container));

    const struct cContainer *class = CLASS(_self);
    assert(class);

    assert(class->drop);
    return class->drop(_self, _element);
}

