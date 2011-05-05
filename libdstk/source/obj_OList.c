#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/obj_OList.h"

#define PARENT_CLASS ((void *)&_AObj)

static void *OList_add(void *_self, void *element);

static void *OList_ctor(const void *class, va_list *app) {
    struct OList *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    new->count = 0;

    void *data;
    while((data = va_arg(*app, void *)))
        OList_add(new, data);

    return new;
}

static void OList_dtor(void *_self) {
    struct OList *self = _self;

    struct list *tmp = self->first, *next;
    while(tmp && self->count>0) {
        next = tmp->next;

        O_REFCNT(tmp->data)--;
        obj_delete(tmp->data);
        free(tmp);

        self->count--;
        tmp = next;
    }
    assert(!self->count);

    cOBJ(PARENT_CLASS)->dtor(_self);
}

static void *OList_clone(const void *_self) {
    struct OList *new = cOBJ(PARENT_CLASS)->clone(_self);

    const struct OList *self = _self;

    new->count = 0;

    struct list *tmp = self->first;
    while(tmp) {
        OList_add(new, obj_clone(tmp->data));

        tmp = tmp->next;
    }

    return new;
}

static int OList_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(PARENT_CLASS)->cmp(_self, _b);
    if(ret) return ret;

    const struct OList *self = _self;
    const struct OList *b = _b;

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

static void *OList_add(void *_self, void *element) {
    struct OList *self = _self;

    assert(element && obj_isclass(element, Obj));

    O_REFCNT(element)++;

    struct list *new = (struct list *)malloc(sizeof(struct list));
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

static void *OList_find(void *_self, void *element) {
    struct OList *self = _self;

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

static void *OList_drop(void *_self, void *element) {
    struct OList *self = _self;

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
    free(curr);
    self->count--;

    O_REFCNT(element)--;
    return element;
}

const struct cOList _OList = {
    {   // AObj
        {   // Obj
            sizeof(struct OList)    /* size */,
            sizeof(struct cOList)   /* csize */,
            CFL_DEFAULTS            /* flags */,
            PARENT_CLASS            /* parent */,
            OList_ctor              /* ctor */,
            OList_dtor              /* dtor */,
            OList_clone             /* clone */,
            OList_cmp               /* cmp */
        },
        NULL                        /* sigemit */
    },
    OList_add                       /* add */,
    OList_find                      /* find */,
    OList_drop                      /* drop */
};

const void *OList = &_OList;

// ---- new functions ----

void *obj_add(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, OList));

    const struct cOList *class = CLASS(_self);
    assert(class);

//    INIT_CLASS(class);

    assert(class->add);
    return class->add(_self, _element);
}

void *obj_find(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, OList));

    const struct cOList *class = CLASS(_self);
    assert(class);

//    INIT_CLASS(class);

    assert(class->find);
    return class->find(_self, _element);
}

void *obj_drop(void *_self, void *_element) {
    assert(_self && obj_isclass(_self, OList));

    const struct cOList *class = CLASS(_self);
    assert(class);

//    INIT_CLASS(class);

    assert(class->drop);
    return class->drop(_self, _element);
}

