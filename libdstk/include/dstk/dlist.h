#ifndef _DLIST_H
#define _DLIST_H

#include <dstk/common.h>

typedef struct DList DList;
struct DList {
    void *data;

    DList *next;
    DList *prev;
};

inline DList *  dlist_alloc         (void *data);
DList *         dlist_append        (DList *list, void *data);
DList *         dlist_prepend       (DList *list, void *data);
// TODO: DList *         dlist_insert_sorted (DList *list, void *data, TKCompareFunc func);
unsigned int    dlist_length        (DList *list);
DList *         dlist_first         (DList *list);
DList *         dlist_last          (DList *list);
DList *         dlist_nth           (DList *list, unsigned int n);
DList *         dlist_find          (DList *list, void *data);
DList *         dlist_find_custom   (DList *list, void *data, TKCompareFunc func);
DList *         dlist_copy          (DList *list);
// TODO: DList *         dlist_reverse       (DList *list);
// TODO: DList *         dlist_sort          (DList *list, TKCompareFunc func);
void            dlist_foreach       (DList *list, TKFunc func, void *user_data);
DList *         dlist_remove        (DList *list, void *data);
DList *         dlist_remove_all    (DList *list, void *data);
inline DList *  dlist_drop          (DList *list, DList *el);
inline void     dlist_free          (DList *el);
DList *         dlist_free_all      (DList *list);

// ------------ DEBUG ------------

#include <stdio.h>

void            dlist_dump          (DList *list, FILE *fd);

#endif

