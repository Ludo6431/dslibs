#ifndef _SLIST_H
#define _SLIST_H

#include <dstk/common.h>

typedef struct SList SList;
struct SList {
    SList *next;
};

#define SLIST_GETDATA(l, o) (*(void **)((char *)(l)+(o)))

inline SList *  slist_alloc                 (size_t sz);
#define         slist_new(t)                (t *)slist_alloc(sizeof(t))
SList *         slist_concat_full           (SList *l1, SList *l2);
#define         slist_append(l, e)          (typeof(l))slist_concat_full((SList *)(l), (SList *)(e))
#define         slist_prepend(l, e)         (typeof(l))slist_concat_full((SList *)(e),(SList *)(l))
#define         slist_concat(l1, l2)        (typeof(l))slist_concat_full((SList *)(l1), (SList *)(l2))
unsigned int    slist_length                (SList *list);
#define         slist_length(l)             slist_length((SList *)(l))
#define         slist_first(l)              (l)
SList *         slist_last                  (SList *list);
#define         slist_last(l)               (typeof(l))slist_last((SList *)(l))
SList *         slist_nth                   (SList *list, unsigned int n);
#define         slist_nth(l, n)             (typeof(l))slist_nth((SList *)(l), (n))
SList *         slist_find                  (SList *list, unsigned int offset, void *data);
#define         slist_find(l, o, d)         (typeof(l))slist_find((SList *)(l), (o), (d))
SList *         slist_find_custom           (SList *list, TKCompareFunc func, void *user_data);
#define         slist_find_custom(l, f, ud)  (typeof(l))slist_find_custom((SList *)(l), (f), (ud))
SList *         slist_copy_full             (size_t sz, SList *list);
#define         slist_copy(l)               (typeof(l))slist_copy_full(sizeof(*(l)), (SList *)(l))
// TODO: SList *         slist_insert_sorted (SList *list, void *data, TKCompareFunc func);
// TODO: SList *         slist_reverse       (SList *list);
// TODO: SList *         slist_sort          (SList *list, TKCompareFunc func);
inline void     slist_foreach               (SList *list, TKFunc func, void *user_data);
#define         slist_foreach(l, f, ud)     slist_foreach((SList *)(l), (f), (ud))
SList *         slist_remove_full           (size_t sz, SList *list, unsigned int offset, void *data);
#define         slist_remove(l, o, d)       (typeof(l))slist_remove_full(sizeof(*(l)), (SList *)(l), (o), (d))
SList *         slist_remove_all_full       (size_t sz, SList *list, unsigned int offset, void *data);
#define         slist_remove_all(l, o, d)   (typeof(l))slist_remove_all_full(sizeof(*(l)), (SList *)(l), (o), (d))
SList *         slist_drop                  (SList *list, SList *el);
#define         slist_drop(l, e)            (typeof(l))slist_drop((SList *)(l), (SList *)(e))
inline SList *  slist_drop_next             (SList *list, SList *el);
#define         slist_drop_next(l, e)       (typeof(l))slist_drop_next((SList *)(l), (SList *)(e))
inline void     slist_free_full             (size_t sz, SList *el);
#define         slist_free(e)               slist_free_full(sizeof(*(e)), (SList *)(e))
SList *         slist_free_all_full         (size_t sz, SList *list);
#define         slist_free_all(l)           (typeof(l))slist_free_all_full(sizeof(*(l)), (SList *)(l))

// ------------ DEBUG ------------

#include <stdio.h>

void            slist_dump_full             (unsigned int sz, SList *list, FILE *fd);
#define         slist_dump(l, fd)           slist_dump_full(sizeof(*(l)), (SList *)(l), (fd))

#endif

