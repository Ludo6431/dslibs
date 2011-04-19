#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/obj_OList.h"

static void *OList_add(void *_self, void *element);

static void *OList_ctor(void *_self, va_list *app) {
    struct OList *self = cOBJ(Obj)->ctor(_self, app);

    self->count = 0;

    void *data;
    while((data = va_arg(*app, void *)))
        OList_add(self, data);

    return self;
}

static void *OList_dtor(void *_self) {
    struct OList *self = _self;

    struct list *tmp = self->first, *next;
    while(tmp) {
        next = tmp->next;

        REFCNT(tmp->data)--;
        obj_delete(tmp->data);
        free(tmp);

        tmp = next;
    }
    self->count = 0;

    return cOBJ(Obj)->dtor(_self);
}

static void *OList_clone(const void *_self) {
    struct OList *new = cOBJ(Obj)->clone(_self);

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
    int ret = cOBJ(Obj)->cmp(_self, _b);
    if(ret) return ret;

    const struct OList *self = _self;
    const struct OList *b = _b;

    if(self->count != b->count)
        return self->count - b->count;

    struct list *tmp1 = self->first, *tmp2 = b->first;
    while(tmp1) {
        ret = cOBJ(Obj)->cmp(tmp1->data, tmp2->data);
        if(ret) return ret;

        tmp1 = tmp1->next;
        tmp2 = tmp2->next;
    }

    return 0;
}

static int OList_isclass(const void *class) {
    return class == OList || cOBJ(Obj)->isclass(class);
}

static void *OList_add(void *_self, void *element) {
    struct OList *self = _self;

    REFCNT(element)++;

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

    if(curr) {
        REFCNT(element)--;

        if(curr == self->first)
            self->first = self->first->next;

        if(curr == self->last)
            self->last = prev;

        if(prev)
            prev->next = curr->next;

        free(curr);

        self->count--;

        return element;
    }

    return NULL;
}

static const struct cOList _OList = {
    {   // Obj
        sizeof(struct OList)    /* size */,
        OList_ctor              /* ctor */,
        OList_dtor              /* dtor */,
        OList_clone             /* clone */,
        OList_cmp               /* cmp */,
        OList_isclass           /* isclass */
    },
    OList_add       /* add */,
    OList_find      /* find */,
    OList_drop      /* drop */
};

const void *OList = &_OList;

// ---- new functions ----

void *obj_add(void *_self, void *_element) {
    if(_self && obj_isclass(_self, OList)) {
        const struct cOList *class = CLASS(_self);
        assert(class && class->add);

        return class->add(_self, _element);
    }

    return NULL;
}

void *obj_find(void *_self, void *_element) {
    if(_self && obj_isclass(_self, OList)) {
        const struct cOList *class = CLASS(_self);
        assert(class && class->find);

        return class->find(_self, _element);
    }

    return NULL;
}

void *obj_drop(void *_self, void *_element) {
    if(_self && obj_isclass(_self, OList)) {
        const struct cOList *class = CLASS(_self);
        assert(class && class->drop);

        return class->drop(_self, _element);
    }

    return NULL;
}

