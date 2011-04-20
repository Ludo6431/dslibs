#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/obj_String.h"

static void *String_ctor(void *_self, va_list *app) {
    const char *text = va_arg(*app, const char *);

    return CTORV(Data, _self, strlen(text)+1 /* datasize */, text /* data */);
}

static void *String_dtor(void *_self) {
    return cOBJ(Data)->dtor(_self);
}

static void *String_clone(const void *_self) {
    return cOBJ(Data)->clone(_self);
}

static int String_cmp(const void *_self, const void *_b) {
    return cOBJ(Data)->cmp(_self, _b);
}

static int String_isclass(const void *class) {
    return class == String || cOBJ(Data)->isclass(class);
}

static int String_repr(const void *_self, char *s, unsigned l) {
    return cDATA(Data)->repr(_self, s, l);
}

static const struct cString _String = {
    {   // Data
        {   // Obj
            sizeof(struct String),
            String_ctor,
            String_dtor,
            String_clone,
            String_cmp,
            String_isclass
        },
        String_repr
    }
};

const void *String = & _String;

// ---- new functions ----

