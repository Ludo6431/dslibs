#ifndef _SLIST_H
#define _SLIST_H

#include <dstk/common.h>

#include <dstk/gslist.h>

typedef struct SList SList;
struct SList {
    SList *next;
    // ----------

    void *data;
};

inline SList *  slist_alloc         (void *data);
SList *         slist_append        (SList *list, void *data);
SList *         slist_prepend       (SList *list, void *data);
SList *         slist_concat        (SList *l1, SList *l2);
unsigned int    slist_length        (SList *list);
inline SList *  slist_first         (SList *list);
#define         slist_first(l)      ((SList *)(l))
SList *         slist_last          (SList *list);
SList *         slist_nth           (SList *list, unsigned int n);
SList *         slist_find          (SList *list, void *data);
SList *         slist_find_custom   (SList *list, void *data, TKCompareFunc func);
SList *         slist_copy          (SList *list);
// TODO: SList *         slist_insert_sorted (SList *list, void *data, TKCompareFunc func);
// TODO: SList *         slist_reverse       (SList *list);
// TODO: SList *         slist_sort          (SList *list, TKCompareFunc func);
inline void     slist_foreach       (SList *list, TKFunc func, void *user_data);
SList *         slist_remove        (SList *list, void *data);
SList *         slist_remove_all    (SList *list, void *data);
SList *         slist_drop          (SList *list, SList *el);
inline SList *  slist_drop_next     (SList *list, SList *elprev);
inline void     slist_free          (SList *el);
SList *         slist_free_all      (SList *list);

// ------------ DEBUG ------------

#include <stdio.h>

void            slist_dump          (SList *list, FILE *fd);

#endif

