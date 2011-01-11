#ifndef _OBJ_OLIST_H
#define _OBJ_OLIST_H

#include "dstk/obj.h"

struct list {
    void *data;

    struct list *next;
};

struct OList {
    struct Obj _;

    unsigned count;
    struct list *first;
    struct list *last;
};
#define OLIST(obj) ((struct OList *)(obj))

struct cOList {
    struct cObj _;

    void *  (*add)     (void *self, void *element);
    void *  (*find)    (void *self, void *element);
    void *  (*drop)    (void *self, void *element);
};
#define cOLIST(cl) ((struct cOList *)(cl))

extern const void *OList;

// ---- new functions ----

void *  obj_add     (void *self, void *element);
void *  obj_find    (void *self, void *element);
void *  obj_drop    (void *self, void *element);

#endif

