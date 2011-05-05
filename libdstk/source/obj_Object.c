#include <stdlib.h>
#include <assert.h>

#include "dstk/obj_Object.h"

#define PARENT_CLASS ((void *)&_OList)

// TODO

const struct cObject _Object = {
    {   // OList
        {   // AObj
            {   // Obj
                sizeof(struct Object)   /* size */,
                sizeof(struct cObject)  /* csize */,
                CFL_DEFAULTS            /* flags */,
                PARENT_CLASS            /* parent */,
                NULL                    /* ctor */,
                NULL                    /* dtor */,
                NULL                    /* clone */,
                NULL                    /* cmp */
            },
            NULL                        /* sigemit */
        },
        NULL                            /* add */,
        NULL                            /* find */,
        NULL                            /* drop */
    }
};

const void *Object = & _Object;

// ---- new functions ----

