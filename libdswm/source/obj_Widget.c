#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "dswm/obj_Widget.h"

#define PARENT_CLASS ((void *)&_Object)

static void *Widget_ctor(const void *class, va_list *app) {
    struct Widget *new = cOBJ(PARENT_CLASS)->ctor(class, app);

    // TODO

    return new;
}

static void Widget_dtor(void *_self) {
    struct Widget *self = _self;

    // TODO

    cOBJ(PARENT_CLASS)->dtor(_self);
}

static void *Widget_clone(const void *_self) {
    struct Widget *new = cOBJ(PARENT_CLASS)->clone(_self);

    const struct Widget *self = _self;

    // TODO

    return new;
}

static int Widget_cmp(const void *_self, const void *_b) {
    int ret = cOBJ(PARENT_CLASS)->cmp(_self, _b);
    if(ret) return ret;

    const struct Widget *self = _self;
    const struct Widget *b = _b;

    // TODO

    return 0;
}

const struct cWidget _Widget = {
    {   // Object
        {   // Obj
            sizeof(struct Widget)   /* size */,
            sizeof(struct cWidget)  /* csize */,
            CFL_DEFAULTS            /* flags */,
            PARENT_CLASS            /* parent */,
            Widget_ctor             /* ctor */,
            Widget_dtor             /* dtor */,
            Widget_clone            /* clone */,
            Widget_cmp              /* cmp */
        },
        NULL                        /* sigemit */
    },
    // TODO
};

const void *Widget = &_Widget;

// ---- new functions ----

// TODO

