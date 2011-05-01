#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/obj_String.h"

#define PARENT_CLASS ((void *)&_Data)

static void *String_ctor(void *_self, va_list *app) {
    const char *text = va_arg(*app, const char *);

    return CTORV(PARENT_CLASS, _self, strlen(text)+1 /* datasize */, text /* data */);
}

const struct cString _String = {
    {   // Data
        {   // PObj
            {   // Obj
                sizeof(struct String)   /* size */,
                sizeof(struct cString)  /* csize */,
                CFL_DEFAULTS            /* flags */,
                PARENT_CLASS            /* parent */,
                String_ctor             /* ctor */,
                NULL                    /* dtor */,
                NULL                    /* clone */,
                NULL                    /* cmp */
            },
            NULL                        /* setp */,
            NULL                        /* getp */,
            NULL                        /* delp */
        },
        NULL                            /* repr */
    }
};

const void *String = & _String;

// ---- new functions ----

