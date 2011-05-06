#ifndef _OBJ_CONTAINER_H
#define _OBJ_CONTAINER_H

#include "dswm/obj_Widget.h"

struct list {
    void *data;

    struct list *next;
};

struct Container {
    struct Widget _;

    unsigned count;
    struct list *first;
    struct list *last;
};
#define CONTAINER(obj) ((struct Container *)(obj))

struct cContainer {
    struct cWidget _;

    void *  (*add)     (void *self, void *element);
    void *  (*find)    (void *self, void *element);
    void *  (*drop)    (void *self, void *element);
};
#define cCONTAINER(cl) ((struct cContainer *)(cl))

extern const struct cContainer _Container;
extern const void *Container;

// ---- new functions ----

void *  obj_add     (void *self, void *element);
void *  obj_find    (void *self, void *element);
void *  obj_drop    (void *self, void *element);

#endif

