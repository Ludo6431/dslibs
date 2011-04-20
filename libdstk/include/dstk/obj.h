#ifndef _OBJ_H
#define _OBJ_H

#include <stdarg.h>

#include "dstk/common.h"

struct Obj {
    const void *class;
    unsigned refcount;
};
#define OBJ(obj) ((struct Obj *)(obj))

struct cObj {
    size_t size;
    void *parent;

    void *  (*ctor)     (void *self, va_list *app);
    void *  (*dtor)     (void *self);
    void *  (*clone)    (const void *self);
    int     (*cmp)      (const void *self, const void *b);
};
#define cOBJ(cl) ((struct cObj *)(cl))

extern const struct cObj _Obj;
extern const void *Obj;

// ---- new functions ----

#define CLASS(obj) (OBJ(obj)->class)
#define C_PARENT(cl) (cOBJ(cl)->parent)
#define O_SIZE(obj) (cOBJ(CLASS(obj))->size)
#define O_REFCNT(obj) (OBJ(obj)->refcount)

void *CTORV(const void *class, void *_self, ...);

void *  obj_new     (const void *class, ...);
void    obj_delete  (void *item);
void *  obj_clone   (void *self);
int     obj_cmp     (const void *self, const void *b);
int     obj_isclass (const void *self, const void *class);

#endif

