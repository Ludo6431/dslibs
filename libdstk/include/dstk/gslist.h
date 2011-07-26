#ifndef _GSLIST_H
#define _GSLIST_H

#include <dstk/common.h>

// generic singly-linked list

typedef struct GSList GSList;
struct GSList {
    GSList *next;
};

#define SLIST_GETDATA(l, o) (*(void **)((char *)(l)+(o)))

inline GSList *     gslist_alloc                    (size_t sz);
#define             gslist_new(t)                   (t *)gslist_alloc(sizeof(t))
inline GSList *     gslist_concat_full              (GSList *l1, GSList *l2);
#define             gslist_append(l, e)             (typeof(l))gslist_concat_full((GSList *)(l), (GSList *)(e))
#define             gslist_prepend(l, e)            (typeof(l))gslist_concat_full((GSList *)(e),(GSList *)(l))
#define             gslist_concat(l1, l2)           (typeof(l1))gslist_concat_full((GSList *)(l1), (GSList *)(l2))
inline unsigned int gslist_length                   (GSList *list);
#define             gslist_length(l)                gslist_length((GSList *)(l))
#define             gslist_first(l)                 ((typeof(l))(GSList *)(l))
inline GSList *     gslist_last                     (GSList *list);
#define             gslist_last(l)                  (typeof(l))gslist_last((GSList *)(l))
inline GSList *     gslist_nth                      (GSList *list, unsigned int n);
#define             gslist_nth(l, n)                (typeof(l))gslist_nth((GSList *)(l), (n))
inline GSList *     gslist_find                     (GSList *list, unsigned int offset, void *data);
#define             gslist_find(l, o, d)            (typeof(l))gslist_find((GSList *)(l), (o), (d))
inline GSList *     gslist_find_custom              (GSList *list, TKCompareFunc func, void *user_data);
#define             gslist_find_custom(l, f, ud)    (typeof(l))gslist_find_custom((GSList *)(l), (f), (ud))
inline GSList *     gslist_copy_full                (size_t sz, GSList *list);
#define             gslist_copy(l)                  (typeof(l))gslist_copy_full(sizeof(*(l)), (GSList *)(l))
// TODO: GSList *         gslist_insert_sorted  (GSList *list, void *data, TKCompareFunc func);
// TODO: GSList *         gslist_reverse        (GSList *list);
// TODO: GSList *         gslist_sort           (GSList *list, TKCompareFunc func);
inline void         gslist_foreach                  (GSList *list, TKFunc func, void *user_data);
#define             gslist_foreach(l, f, ud)        gslist_foreach((GSList *)(l), (f), (ud))
inline GSList *     gslist_remove_full              (size_t sz, GSList *list, unsigned int offset, void *data);
#define             gslist_remove(l, o, d)          (typeof(l))gslist_remove_full(sizeof(*(l)), (GSList *)(l), (o), (d))
inline GSList *     gslist_remove_all_full          (size_t sz, GSList *list, unsigned int offset, void *data);
#define             gslist_remove_all(l, o, d)      (typeof(l))gslist_remove_all_full(sizeof(*(l)), (GSList *)(l), (o), (d))
inline GSList *     gslist_drop                     (GSList *list, GSList *el);
#define             gslist_drop(l, e)               (typeof(l))gslist_drop((GSList *)(l), (GSList *)(e))
inline GSList *     gslist_drop_next                (GSList *list, GSList *elprev);
#define             gslist_drop_next(l, e)          (typeof(l))gslist_drop_next((GSList *)(l), (GSList *)(e))
inline void         gslist_free_full                (size_t sz, GSList *el);
#define             gslist_free(e)                  gslist_free_full(sizeof(*(e)), (GSList *)(e))
inline GSList *     gslist_free_all_full            (size_t sz, GSList *list);
#define             gslist_free_all(l)              (typeof(l))gslist_free_all_full(sizeof(*(l)), (GSList *)(l))

// ------------ DEBUG ------------

#include <stdio.h>

inline void         gslist_dump_full                (unsigned int sz, GSList *list, FILE *fd);
#define             gslist_dump(l, fd)              gslist_dump_full(sizeof(*(l)), (GSList *)(l), (fd))

#endif

