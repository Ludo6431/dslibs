#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dstk/obj_Object.h"

#define PARENT_CLASS ((void *)&_Obj)

static void *Object_ctor(const void *class, va_list *app) {
    struct Object *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    new->p_count = 0;
    new->s_count = 0;

    return new;
}

static void Object_dtor(void *_self) {
    struct Object *self = _self;

    obj_sigemit(_self, SIG_DESTROYED);

    // properties
    if(self->p_count) {
        struct prop *pr = self->p_first, *next;
        while(pr) {
            next = pr->next;

            ad_del(pr->data, pr->datasize);
            free(pr);

            pr = next;
        }
    }

    // signals
    if(self->s_count) {
        struct sig *s = self->s_first, *next;
        while(s) {
            next = s->next;

            if(s->count) {
                free(s->handlers);
                free(s->userdata);
            }
            free(s);

            s = next;
        }
    }

    cOBJ(PARENT_CLASS)->dtor(_self);
}

static void *Object_clone(const void *_self) {
    struct Object *new = cOBJ(PARENT_CLASS)->clone(_self);
    const struct Object *self = _self;

    // properties
    new->p_count = 0;
    if(self->p_count) {
        struct prop *pr = self->p_first;
        while(pr) {
            obj_setprop(new, pr->pid, ad_get(pr->data, pr->datasize), pr->datasize);

            pr = pr->next;
        }
    }

    // signals
    new->s_count = 0;   // don't copy signals handlers

    obj_sigemit((void *)_self, SIG_CLONED, new);

    return new;
}

static int Object_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(PARENT_CLASS)->cmp(_self, _b);
    if(ret) return ret;

    const struct Object *self = _self;
    const struct Object *b = _b;

    // properties
    if(self->p_count != b->p_count)
        return self->p_count - b->p_count;

    struct prop *pr1 = self->p_first, *pr2 = b->p_first;    // TODO : the order doesn't matter
    while(pr1) {
        if(pr1->datasize != pr2->datasize)
            return pr1->datasize - pr2->datasize;

        int ret = memcmp(ad_get(pr1->data, pr1->datasize), ad_get(pr2->data, pr2->datasize), pr1->datasize);
        if(ret)
            return ret;

        pr1 = pr1->next;
        pr2 = pr2->next;
    }

    // signals

    return 0;
}

void Object_sigemit(void *_self, unsigned sid, va_list *app) {
    struct Object *self = _self;

    struct sig *s = NULL;
    if(self->s_count) {
        s = self->s_first;
        while(s) {
            if(s->sid == sid)
                break;

            s = s->next;
        }
    }

    if(!s || !s->count)
        return;

    int i;
    struct Object *tmp;
    unsigned id, datasize;
    void *data;

    switch(s->sid) {
    case SIG_CLONED:
        tmp = va_arg(*app, struct Object *);

        for(i=0; i<s->count; i++)
            if(s->handlers[i])
                ((void (*)(void *, void *, void *))s->handlers[i])(self, s->userdata[i], tmp);
        break;

    case SIG_DESTROYED:
        for(i=0; i<s->count; i++)
            if(s->handlers[i])
                ((void (*)(void *, void *))s->handlers[i])(self, s->userdata[i]);
        break;

    case SIG_SETPROP:
        id = va_arg(*app, unsigned);
        data = va_arg(*app, void *);
        datasize = va_arg(*app, unsigned);

        for(i=0; i<s->count; i++)
            if(s->handlers[i])
                ((void (*)(void *, void *, unsigned, void *, unsigned))s->handlers[i])(self, s->userdata[i], id, data, datasize);
        break;

    case SIG_GETPROP:
    case SIG_DELPROP:
        id = va_arg(*app, unsigned);

        for(i=0; i<s->count; i++)
            if(s->handlers[i])
                ((void (*)(void *, void *, unsigned))s->handlers[i])(self, s->userdata[i], id);
        break;

    default:
        break;
    }
}

const struct cObject _Object = {
    {   // Obj
        sizeof(struct Object) /* size */,
        sizeof(struct cObject)/* csize */,
        CFL_DEFAULTS        /* flags */,
        PARENT_CLASS        /* parent */,
        Object_ctor           /* ctor */,
        Object_dtor           /* dtor */,
        Object_clone          /* clone */,
        Object_cmp            /* cmp */
    },
    Object_sigemit            /* sigemit */
};

const void *Object = &_Object;

// ---- new functions ----

void obj_setprop(void *_self, unsigned pid, void *data, unsigned datasize) {
    assert(_self && obj_isclass(_self, Object));

    struct Object *self = _self;

    obj_sigemit(_self, SIG_SETPROP, pid, data, datasize);

    struct prop *pr = NULL;
    if(self->p_count) {
        pr = self->p_first;
        while(pr) {
            if(pr->pid == pid)
                break;

            pr = pr->next;
        }

        if(pr)
            ad_del(pr->data, pr->datasize);
        else {
            pr = self->p_last->next = calloc(1, sizeof(struct prop));
            assert(pr);
            self->p_last = self->p_last->next;
            self->p_count++;
        }
    }
    else {
        pr = self->p_first = self->p_last = calloc(1, sizeof(struct prop));
        assert(pr);
        self->p_count++;
    }

    pr->pid = pid;
    pr->data = ad_new(data, datasize);
    pr->datasize = datasize;
}

void *obj_getprop(void *_self, unsigned pid, unsigned *datasize) {
    assert(_self && obj_isclass(_self, Object));

    struct Object *self = _self;

    obj_sigemit(_self, SIG_GETPROP, pid);

    if(!self->p_count)
        return NULL;

    struct prop *pr = self->p_first;
    while(pr) {
        if(pr->pid == pid)
            break;

        pr = pr->next;
    }

    if(!pr)
        return NULL;

    if(datasize)
        *datasize = pr->datasize;
    return ad_get(pr->data, pr->datasize);
}

void obj_delprop(void *_self, unsigned pid) {
    assert(_self && obj_isclass(_self, Object));

    struct Object *self = _self;

    obj_sigemit(_self, SIG_DELPROP, pid);

    if(!self->p_count)
        return;

    struct prop *pr = self->p_first, *prev = NULL;
    while(pr) {
        if(pr->pid == pid)
            break;

        prev = pr;
        pr = pr->next;
    }

    if(!pr)
        return;

    if(pr == self->p_first)
        self->p_first = self->p_first->next;
    if(pr == self->p_last)
        self->p_last = prev;
    if(prev)
        prev->next = pr->next;

    ad_del(pr->data, pr->datasize);
    free(pr);

    self->p_count--;
}

void obj_sigconnect(void *_self, unsigned sid, Obj_CB cb, void *userdata) {
    assert(_self && obj_isclass(_self, Object));
    assert(cb);

    struct Object *self = _self;

    struct sig *s = NULL;
    if(self->s_count) {
        s = self->s_first;
        while(s) {
            if(s->sid == sid)
                break;

            s = s->next;
        }

        if(!s) {
            s = self->s_last->next = calloc(1, sizeof(struct sig));
            assert(s);
            self->s_last = self->s_last->next;
            self->s_count++;
        }
    }
    else {
        s = self->s_first = self->s_last = calloc(1, sizeof(struct sig));
        assert(s);
        self->s_count++;
    }

    s->sid = sid;
    if(s->count) {
        int i;
        for(i=0; i<s->count; i++)
            if(!s->handlers[i]) {
                s->handlers[i] = cb;
                s->userdata[i] = userdata;
                break;
            }

        if(i==s->count) {
            s->count+=2;
            s->handlers = (void **)realloc(s->handlers, s->count * sizeof(void *));
            assert(s->handlers);
            s->handlers[s->count-1] = cb;
            s->handlers[s->count-2] = NULL;
            s->userdata = (void **)realloc(s->userdata, s->count * sizeof(void *));
            assert(s->userdata);
            s->userdata[s->count-1] = userdata;
            s->userdata[s->count-2] = NULL;
        }
    }
    else {
        s->count = 2;
        s->handlers = (void **)calloc(s->count, sizeof(void *));
        assert(s->handlers);
        s->handlers[0] = cb;
        s->userdata = (void **)calloc(s->count, sizeof(void *));
        assert(s->userdata);
        s->userdata[0] = userdata;
    }
}

void obj_sigemit(void *_self, unsigned sid, ...) {
    assert(_self && obj_isclass(_self, Object));

    const struct cObject *class = CLASS(_self);
    assert(class);

//    INIT_CLASS(class);

    assert(class->sigemit);
    va_list ap;
    va_start(ap, sid);
    class->sigemit(_self, sid, &ap);
    va_end(ap);
}

void obj_sigdisconnect(void *_self, unsigned sid, void *cb) {
    assert(_self && obj_isclass(_self, Object));

    struct Object *self = _self;

    if(!self->s_count)
        return;

    struct sig *s = self->s_first, *prev = NULL;
    while(s) {
        if(s->sid == sid)
            break;

        prev = s;
        s = s->next;
    }

    if(!s)
        return;

    if(s == self->s_first)
        self->s_first = self->s_first->next;
    if(s == self->s_last)
        self->s_last = prev;
    if(prev)
        prev->next = s->next;

    if(s->count) {
        free(s->handlers);
        free(s->userdata);
    }
    free(s);

    self->s_count--;
}

