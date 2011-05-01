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
    unsigned char size;
    unsigned char csize;
    unsigned short flags;
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

enum CFL {
    CFL_INIT = BIT(0),
    NUM_CFL
};

#define CFL_DEFAULTS (0)

#define C_SIZE(cl)      (cOBJ(cl)->size)
#define C_CSIZE(cl)     (cOBJ(cl)->csize)
#define C_FLAGS(cl)     (cOBJ(cl)->flags)
#define C_PARENT(cl)    (cOBJ(cl)->parent)
#define CLASS(obj)      (OBJ(obj)->class)
#define O_SIZE(obj)     C_SIZE(CLASS(obj))
#define O_REFCNT(obj)   (OBJ(obj)->refcount)

void _init_handlers(void *class);
#define INIT_CLASS(cl) \
    do { \
        if(!(C_FLAGS(cl)&CFL_INIT)) \
            _init_handlers((void *)cl); \
    } while(0)

void *CTORV(const void *class, void *_self, ...);

void *  obj_new     (const void *class, ...);
void    obj_delete  (void *item);
void *  obj_clone   (void *self);
int     obj_cmp     (const void *self, const void *b);
int     obj_isclass (const void *self, const void *class);

#endif

