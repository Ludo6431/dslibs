#ifndef _OBJ_WIDGET_H
#define _OBJ_WIDGET_H

#include <dstk/obj_Object.h>

struct Widget {
    struct Object _;

    // TODO
};
#define WIDGET(obj) ((struct Widget *)(obj))

struct cWidget {
    struct cObject _;

    // TODO
};
#define cWIDGET(cl) ((struct cWidget *)(cl))

extern const struct cWidget _Widget;
extern const void *Widget;

// ---- new functions ----

#endif

