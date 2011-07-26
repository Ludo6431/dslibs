#include <stdlib.h>
#include <stdio.h>
#include <nds.h>

#include "procs/procs.h"

typedef struct vbllist_t vbllist_t;
struct vbllist_t {
    struct vbllist_t *next;
    // -----------

    TKFunc hnd;
    void *user_data;
};

vbllist_t *vbllist = NULL;

void timeout_add_vbl(TKFunc fn, void *user_data) {
    if(!fn)
        return;

    vbllist_t *new = gslist_new(vbllist_t);
    new->hnd = fn;
    new->user_data = user_data;

    vbllist = gslist_prepend(vbllist, new);
}

void timeout_remove_vbl(TKFunc fn) {
    vbllist = gslist_remove(vbllist, 4, fn);
}

inline void timeout_run_vbl() {
    vbllist_t *hnd;

    for(hnd = vbllist; hnd; hnd = hnd->next)
        hnd->hnd(hnd->user_data, NULL);
}

/*
typedef struct tmrlist_t tmrlist_t;
struct tmrlist_t {
    struct tmrlist_t *next;
    // -----------

    u16 reload;
    float sec;

    TKFunc hnd;
    void *user_data;
};

tmrlist_t *tmrlist = NULL;

void timeout_add_tmr(u16 reload, TKFunc fn, void *user_data) {
    if(!fn)
        return;

    tmrlist_t *new = gslist_new(tmrlist_t);
    new->reload = reload;
    new->sec = (float)(0x10000 - reload)*1024.0/(float)BUS_CLOCK;
    new->hnd = fn;
    new->user_data = user_data;

    tmrlist = gslist_prepend(tmrlist, new);
}

void timeout_remove_tmr(TKFunc fn) {
    tmrlist = gslist_remove(tmrlist, 12, fn);
}*/

