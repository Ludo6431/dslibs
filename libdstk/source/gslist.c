#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/slice.h"

#include "dstk/gslist.h"

// #define gslist_new(t) (t *)gslist_alloc(sizeof(t))
inline GSList *gslist_alloc(size_t sz) {
    GSList *el = (GSList *)slice_alloc(sz);
    el->next = NULL;
    return el;
}

// #define gslist_append(l, e) (typeof(l))gslist_concat_full((GSList *)(l), (GSList *)(e))
// #define gslist_prepend(l, e) (typeof(l))gslist_concat_full((GSList *)(e),(GSList *)(l))
// #define gslist_concat(l1, l2) (typeof(l))gslist_concat_full((GSList *)(l1), (GSList *)(l2))
inline GSList *gslist_concat_full(GSList *l1, GSList *l2) {
    GSList *l1last = gslist_last(l1);

    if(!l1last)
        return l2;

    l1last->next = l2;

    return l1;
}

#undef gslist_length
// #define gslist_length(l) gslist_length((GSList *)(l))
inline unsigned int gslist_length(GSList *list) {
    unsigned int length = 0;

    while(list)
        list = list->next, length++;

    return length;
}

#undef gslist_first
// #define gslist_first(l) ((GSList *)(l))

#undef gslist_last
// #define gslist_last(l) (typeof(l))gslist_last((GSList *)(l))
inline GSList *gslist_last(GSList *list) {
    if(!list)
        return NULL;

    while(list->next)
        list = list->next;

    return list;
}

#undef gslist_nth
// #define gslist_nth(l, n) (typeof(l))gslist_nth((GSList *)(l), (n))
inline GSList *gslist_nth(GSList *list, unsigned int n) {
    while(n-- && list)
        list = list->next;

    return list;
}

#undef gslist_find
// #define gslist_find(l, o, d) (typeof(l))gslist_find((GSList *)(l), (o), (d))
inline GSList *gslist_find(GSList *list, unsigned int offset, void *data) {
    while(list) {
        if(SLIST_GETDATA(list, offset) == data)
            return list;

        list = list->next;
    }

    return NULL;
}

#undef gslist_find_custom
// #define gslist_find_custom(l, f, ud) (typeof(l))gslist_find_custom((GSList *)(l), (f), (ud))
inline GSList *gslist_find_custom(GSList *list, TKCompareFunc func, void *user_data) {
    assert(func);

    while(list) {
        if(!func(list, user_data))
            return list;

        list = list->next;
    }

    return NULL;
}

// #define gslist_copy(l) (typeof(l))gslist_copy_full(sizeof(*(l)), (GSList *)(l))
inline GSList *gslist_copy_full(size_t sz, GSList *list) {
    GSList *_list, *ret = NULL;

    if(!list)
        return NULL;

    ret = _list = slice_copy(sz, list);
    list = list->next;

    while(list) {
        _list->next = slice_copy(sz, list);

        _list = _list->next;
        list = list->next;
    }

    _list->next = NULL;

    return ret;
}

// TODO: GSList *         gslist_insert_sorted (GSList *list, void *data, TKCompareFunc func);
// TODO: GSList *         gslist_reverse       (GSList *list);
// TODO: GSList *         gslist_sort          (GSList *list, TKCompareFunc func);

#undef gslist_foreach
// #define gslist_foreach(l, f, ud) gslist_foreach((GSList *)(l), (f), (ud))
inline void gslist_foreach(GSList *list, TKFunc func, void *user_data) {
    assert(func);

    while(list)
        list = (GSList *)func(list, user_data);
}

// #define gslist_remove(l, o, d) (typeof(l))gslist_remove_full(sizeof(*(l)), (GSList *)(l), (o), (d))
inline GSList *gslist_remove_full(size_t sz, GSList *list, unsigned int offset, void *data) {
    GSList *el = list, *elprev = NULL;

    while(el) {
        if(SLIST_GETDATA(el, offset) == data) {
            if(elprev)
                elprev->next = el->next;
            else
                list = list->next;

            gslist_free_full(sz, el);

            break;
        }

        elprev = el;
        el = el->next;
    }

    return list;
}

// #define gslist_remove_all(l, o, d) (typeof(l))gslist_remove_all_full(sizeof(*(l)), (GSList *)(l), (o), (d))
inline GSList *gslist_remove_all_full(size_t sz, GSList *list, unsigned int offset, void *data) {
    GSList *el = list, *elnext, *elprev = NULL;

    while(el) {
        elnext = el->next;

        if(SLIST_GETDATA(el, offset) == data) {
            if(elprev)
                elprev->next = elnext;
            else
                list = list->next;

            gslist_free_full(sz, el);
        }
        else
            elprev = el;

        el = elnext;
    }

    return list;
}

#undef gslist_drop
// #define gslist_drop(l, e) (typeof(l))gslist_drop((GSList *)(l), (e))
inline GSList *gslist_drop(GSList *list, GSList *del) {
    GSList *el = list, *elprev = NULL;

    while(el) {
        if(el == del) {
            if(elprev)
                elprev->next = el->next;
            else
                list = list->next;

            el->next = NULL;

            break;
        }

        elprev = el;
        el = el->next;
    }

    return list;
}

#undef gslist_drop_next
// #define gslist_drop_next(l, e) (typeof(l))gslist_drop_next((GSList *)(l), (e))
inline GSList *gslist_drop_next(GSList *list, GSList *elprev) {
    if(!list)
        return NULL;

    if(elprev) {
        if(elprev->next)
            elprev->next = elprev->next->next;
    }
    else
        list = list->next;

    return list;
}

// #define gslist_free(e) gslist_free_full(sizeof(*(e)), (e))
inline void gslist_free_full(size_t sz, GSList *el) {
    slice_free(sz, el);
}

// #define gslist_free_all(l) (typeof(l))gslist_free_all_full(sizeof(*(l)), (GSList *)(l))
inline GSList *gslist_free_all_full(size_t sz, GSList *list) {
    GSList *el = list, *elnext;

    while(el) {
        elnext = el->next;

        gslist_free_full(sz, el);

        el = elnext;
    }

    return NULL;
}

// ------------ DEBUG ------------

// #define gslist_dump(l, fd) gslist_dump_full(sizeof(*(l)), (GSList *)(l), (fd))
inline void gslist_dump_full(unsigned int sz, GSList *list, FILE *fd) {
    GSList *el = list;
    unsigned int i = 0, j;

    fprintf(fd, "%dB list\n", sz);

    while(el) {
        fprintf(fd, "El #%2d: ", ++i);

        for(j=sizeof(GSList); j<sz; j++)
            fprintf(fd, "%02x", ((char *)el)[j]);

        fprintf(fd, " (%8p->%8p)\n", el, el->next);

        el = el->next;
    }
}

