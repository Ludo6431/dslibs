#ifndef _OBJ_OBJECT_H
#define _OBJ_OBJECT_H

#include <stdarg.h>

#include <dstk/signals.h>
#include <dstk/adata.h>

#include <dstk/obj.h>

struct prop {
    unsigned pid;

    adata_t data;
    unsigned datasize;

    struct prop *next;
};

struct sig {
    unsigned sid;

    unsigned count;
    void **handlers;
    void **userdata;

    struct sig *next;
};

struct Object {
    struct Obj _;

    // properties
    unsigned p_count;
    struct prop *p_first;
    struct prop *p_last;

    // signals
    unsigned s_count;
    struct sig *s_first;
    struct sig *s_last;
};
#define OBJECT(obj) ((struct Object *)(obj))

struct cObject {
    struct cObj _;

    void    (*sigemit)  (void *self, unsigned sid, va_list *app);
};
#define cOBJECT(cl) ((struct cAObj *)(cl))

extern const struct cObject _Object;
extern const void *Object;

// ---- new functions ----

// properties
void    obj_setprop (void *self, unsigned pid, void *data, unsigned datasize);
void *  obj_getprop (void *self, unsigned pid, unsigned *datasize);
void    obj_delprop (void *self, unsigned pid);

// signals
typedef void *(*Obj_CB)(void *self, void *userdata);

void    obj_sigconnect      (void *self, unsigned sid, Obj_CB cb, void *userdata);
void    obj_sigemit         (void *self, unsigned sid, ...);
void    obj_sigdisconnect   (void *self, unsigned sid, void *cb);

#endif

