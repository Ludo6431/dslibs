#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/slice.h"

#include "dstk/slist.h"

// #define slist_new(t) (t *)slist_alloc(sizeof(t))
inline SList *slist_alloc(size_t sz) {
    SList *el = (SList *)slice_alloc(sz);
    el->next = NULL;
    return el;
}

// #define slist_append(l, e) (typeof(l))slist_concat_full((SList *)(l), (SList *)(e))
// #define slist_prepend(l, e) (typeof(l))slist_concat_full((SList *)(e),(SList *)(l))
// #define slist_concat(l1, l2) (typeof(l))slist_concat_full((SList *)(l1), (SList *)(l2))
SList *slist_concat_full(SList *l1, SList *l2) {
    SList *l1last = slist_last(l1);

    if(!l1last)
        return l2;

    l1last->next = l2;

    return l1;
}

#undef slist_length
// #define slist_length(l) slist_length((SList *)(l))
unsigned int slist_length(SList *list) {
    unsigned int length = 0;

    while(list)
        list = list->next, length++;

    return length;
}

#undef slist_first
// #define slist_first(l) (l)

#undef slist_last
// #define slist_last(l) (typeof(l))slist_last((SList *)(l))
SList *slist_last(SList *list) {
    if(!list)
        return NULL;

    while(list->next)
        list = list->next;

    return list;
}

#undef slist_nth
// #define slist_nth(l, n) (typeof(l))slist_nth((SList *)(l), (n))
SList *slist_nth(SList *list, unsigned int n) {
    while(n-- && list)
        list = list->next;

    return list;
}

#undef slist_find
// #define slist_find(l, o, d) (typeof(l))slist_find((SList *)(l), (o), (d))
SList *slist_find(SList *list, unsigned int offset, void *data) {
    while(list) {
        if(SLIST_GETDATA(list, offset) == data)
            return list;

        list = list->next;
    }

    return NULL;
}

#undef slist_find_custom
// #define slist_find_custom(l, f, ud) (typeof(l))slist_find_custom((SList *)(l), (f), (ud))
SList *slist_find_custom(SList *list, TKCompareFunc func, void *user_data) {
    assert(func);

    while(list) {
        if(!func(list, user_data))
            return list;

        list = list->next;
    }

    return NULL;
}

// #define slist_copy(l) (typeof(l))slist_copy_full(sizeof(*(l)), (SList *)(l))
SList *slist_copy_full(size_t sz, SList *list) {
    SList *_list, *ret = NULL;

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

// TODO: SList *         slist_insert_sorted (SList *list, void *data, TKCompareFunc func);
// TODO: SList *         slist_reverse       (SList *list);
// TODO: SList *         slist_sort          (SList *list, TKCompareFunc func);

#undef slist_foreach
// #define slist_foreach(l, f, ud) slist_foreach((SList *)(l), (f), (ud))
inline void slist_foreach(SList *list, TKFunc func, void *user_data) {
    assert(func);

    while(list)
        list = (SList *)func(list, user_data);
}

// #define slist_remove(l, o, d) (typeof(l))slist_remove_full(sizeof(*(l)), (SList *)(l), (o), (d))
SList *slist_remove_full(size_t sz, SList *list, unsigned int offset, void *data) {
    SList *el = list, *elprev = NULL;

    while(el) {
        if(SLIST_GETDATA(el, offset) == data) {
            if(elprev)
                elprev->next = el->next;
            else
                list = list->next;

            slist_free_full(sz, el);

            break;
        }

        elprev = el;
        el = el->next;
    }

    return list;
}

// #define slist_remove_all(l, o, d) (typeof(l))slist_remove_all_full(sizeof(*(l)), (SList *)(l), (o), (d))
SList *slist_remove_all_full(size_t sz, SList *list, unsigned int offset, void *data) {
    SList *el = list, *elnext, *elprev = NULL;

    while(el) {
        elnext = el->next;

        if(SLIST_GETDATA(el, offset) == data) {
            if(elprev)
                elprev->next = elnext;
            else
                list = list->next;

            slist_free_full(sz, el);
        }
        else
            elprev = el;

        el = elnext;
    }

    return list;
}

#undef slist_drop
// #define slist_drop(l, e) (typeof(l))slist_drop((SList *)(l), (e))
SList *slist_drop(SList *list, SList *del) {
    SList *el = list, *elprev = NULL;

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

#undef slist_drop_next
// #define slist_drop_next(l, e) (typeof(l))slist_drop_next((SList *)(l), (e))
inline SList *slist_drop_next(SList *list, SList *elprev) {
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

// #define slist_free(e) slist_free_full(sizeof(*(e)), (e))
inline void slist_free_full(size_t sz, SList *el) {
    slice_free(sz, el);
}

// #define slist_free_all(l) (typeof(l))slist_free_all_full(sizeof(*(l)), (SList *)(l))
SList *slist_free_all_full(size_t sz, SList *list) {
    SList *el = list, *elnext;

    while(el) {
        elnext = el->next;

        slist_free_full(sz, el);

        el = elnext;
    }

    return NULL;
}

// ------------ DEBUG ------------

// #define slist_dump(l, fd) slist_dump_full(sizeof(*(l)), (SList *)(l), (fd))
void slist_dump_full(unsigned int sz, SList *list, FILE *fd) {
    SList *el = list;
    unsigned int i = 0, j;

    fprintf(fd, "%dB list\n", sz);

    while(el) {
        fprintf(fd, "El#%d: ", ++i);

        for(j=sizeof(SList); j<sz; j++)
            fprintf(fd, "%02x", ((char *)el)[j]);

        fprintf(fd, " (%p->%p)\n", el, el->next);

        el = el->next;
    }
}

