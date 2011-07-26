#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/slist.h"

inline SList *slist_alloc(void *data) {
    SList *el = gslist_new(SList);
    if(el)
        el->data = data;
    return el;
}

SList *slist_append(SList *list, void *data) {
    return gslist_append(list, slist_alloc(data));
}

SList *slist_prepend(SList *list, void *data) {
    return gslist_prepend(list, slist_alloc(data));
}

SList *slist_concat(SList *l1, SList *l2) {
    return gslist_concat(l1, l2);
}

unsigned int slist_length(SList *list) {
    return gslist_length(list);
}

#undef slist_first
// #define slist_first(l) ((SList *)(l))
inline SList *slist_first(SList *list) {
    return gslist_first(list);
}

SList *slist_last(SList *list) {
    return gslist_last(list);
}

SList *slist_nth(SList *list, unsigned int n) {
    return gslist_nth(list, n);
}

SList *slist_find(SList *list, void *data) {
    return gslist_find(list, 4, data);
}

SList *slist_find_custom(SList *list, void *data, TKCompareFunc func) {
    inline int mfunc(SList *l, void *data) {
        return func(l->data, data);
    }

    return gslist_find_custom(list, (TKCompareFunc)mfunc, data);
}

SList *slist_copy(SList *list) {
    return gslist_copy(list);
}

// TODO: SList *         slist_insert_sorted (SList *list, void *data, TKCompareFunc func);
// TODO: SList *         slist_reverse       (SList *list);
// TODO: SList *         slist_sort          (SList *list, TKCompareFunc func);

inline void slist_foreach(SList *list, TKFunc func, void *user_data) {
    inline SList *mfunc(SList *l, void *user_data) {
        func(l->data, user_data);

        return l->next;
    }

    return gslist_foreach(list, (TKFunc)mfunc, user_data);
}

SList *slist_remove(SList *list, void *data) {
    return gslist_remove(list, 4, data);
}

SList *slist_remove_all(SList *list, void *data) {
    return gslist_remove_all(list, 4, data);
}

SList *slist_drop(SList *list, SList *el) {
    return gslist_drop(list, el);
}

inline SList *slist_drop_next(SList *list, SList *elprev) {
    return gslist_drop_next(list, elprev);
}

inline void slist_free(SList *el) {
    gslist_free(el);
}

SList *slist_free_all(SList *list) {
    return gslist_free_all(list);
}

// ------------ DEBUG ------------

void slist_dump(SList *list, FILE *fd) {
    SList *el = list;
    unsigned int i = 0;

    while(el) {
        fprintf(fd, "SList #%2d: %8p (%8p->%8p)", ++i, el->data, el, el->next);

        el = el->next;
    }
}

