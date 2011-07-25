#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/slice.h"

#include "dstk/dlist.h"

inline DList *dlist_alloc(void *data) {
    DList *list = slice_new0(DList);
    list->data = data;
    return list;
}

DList *dlist_append(DList *_list, void *data) { // note that this function traverses the entire list
    DList *list = dlist_last(_list);

    if(!list)
        return dlist_alloc(data);

    list->next = dlist_alloc(data);
    list->next->prev = list;

    return _list;
}

DList *dlist_prepend(DList *list, void *data) {
    DList *_list = dlist_alloc(data);
    list = dlist_first(list);

    if(!list)
        return _list;

    _list->next = list;
    list->prev = _list;

    return _list;
}

unsigned int dlist_length(DList *list) {
    unsigned int length = 0;

    while(list)
        list = list->next, length++;

    return length;
}

DList *dlist_first(DList *list) {
    if(!list)
        return NULL;

    while(list->prev)
        list = list->prev;

    return list;
}

DList *dlist_last(DList *list) {    // note that this function traverses the entire list
    if(!list)
        return NULL;

    while(list->next)
        list = list->next;

    return list;
}

DList *dlist_nth(DList *list, unsigned int n) {
    while(n-- && list)
        list = list->next;

    return list;
}

DList *dlist_find(DList *list, void *data) {
    while(list) {
        if(list->data == data)
            return list;

        list = list->next;
    }

    return NULL;
}

DList *dlist_find_custom(DList *list, void *data, TKCompareFunc func) {
    assert(func);

    while(list) {
        if(!func(list->data, data))
            return list;

        list = list->next;
    }

    return NULL;
}

DList *dlist_copy(DList *list) {
    DList *_list, *ret = NULL;

    if(!list)
        return NULL;

    ret = _list = slice_new(DList);
    _list->prev = NULL;
    _list->data = list->data;
    list = list->next;

    while(list) {
        _list->next = slice_new(DList);
        _list->next->prev = _list;
        _list->next->data = list->data;

        _list = _list->next;
        list = list->next;
    }

    _list->next = NULL;

    return ret;
}

// TODO: DList *         dlist_insert_sorted (DList *list, void *data, TKCompareFunc func);
// TODO: DList *         dlist_reverse       (DList *list);
// TODO: DList *         dlist_sort          (DList *list, TKCompareFunc func);

void dlist_foreach(DList *list, TKFunc func, void *user_data) {
    assert(func);

    while(list) {
        func(list->data, user_data);

        list = list->next;
    }
}

DList *dlist_remove(DList *list, void *data) {
    DList *el = list;

    while(el) {
        if(el->data == data) {
            list = dlist_drop(list, el);
            dlist_free(el);

            break;
        }

        el = el->next;
    }

    return list;
}

DList *dlist_remove_all(DList *list, void *data) {
    DList *el = list, *elnext;

    while(el) {
        elnext = el->next;

        if(el->data == data) {
            list = dlist_drop(list, el);
            dlist_free(el);
        }

        el = elnext;
    }

    return list;
}

inline DList *dlist_drop(DList *list, DList *el) {
    if(el->prev)
        el->prev->next = el->next;
    else {
        assert(el == list);
        list = list->next;
    }

    if(el->next)
        el->next->prev = el->prev;

    el->prev = NULL;
    el->next = NULL;

    return list;
}

inline void dlist_free(DList *el) {
    slice_delete(DList, el);
}

DList *dlist_free_all(DList *list) {
    DList *el = list, *elnext;

    while(el) {
        elnext = el->next;

        dlist_free(el);

        el = elnext;
    }

    return NULL;
}

// ------------ DEBUG ------------

void dlist_dump(DList *list, FILE *fd) {
    DList *el = list;
    unsigned int i = 0;

    while(el) {
        fprintf(fd, "El#%d: %p (%p<-%p->%p)\n", ++i, el->data, el->prev, el, el->next);

        el = el->next;
    }
}

