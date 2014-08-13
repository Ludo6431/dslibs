#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "list.h"

inline sLEl *L_new_el(void *data) {
    sLEl *el = (sLEl *)calloc(1, sizeof(sLEl));
    if(!el) return NULL;

    el->data = data;

    return el;
}

void *L_add_head(sList *list, void *data) {
    assert(list);

    sLEl *el = L_new_el(data);
    if(!el) return NULL;

    if(list->count) {
        assert(list->head);

        el->next = list->head;
        list->head->prev = el;
        list->head = list->head->prev;
    }
    else
        list->head = list->tail = el;

    list->count++;

    return data;
}

void *L_add_tail(sList *list, void *data) {
    assert(list);

    sLEl *el = L_new_el(data);
    if(!el) return NULL;

    if(list->count) {
        assert(list->tail);

        el->prev = list->tail;
        list->tail->next = el;
        list->tail = list->tail->next;
    }
    else
        list->head = list->tail = el;

    list->count++;

    return data;
}

/*void *L_insert_before_el(sList *list, sLEl *element, void *data)*/

void *L_insert_after_el(sList *list, sLEl *element, void *data) {
    assert(list);

    if(!element) return L_add_head(list, data);
    if(element == list->tail) return L_add_tail(list, data);

    sLEl *el = L_new_el(data);
    if(!el) return NULL;

    el->prev = element;
    el->next = element->next;
    element->next->prev = el;
    element->next = el;

    list->count++;

    return data;
}

void L_foreach(sList *list, datahandler hnd) {
    assert(list);
    assert(hnd);

    if(!list->count) return;

    sLEl *curr;
    for(curr = list->head; curr; curr = curr->next)
        hnd(curr->data);
}

inline void *_memdup(void *src, unsigned size) {
    if(!size) return src;

    assert(src);

    void *dst = malloc(size);
    if(!dst) return NULL;

    memcpy(dst, src, size);

    return dst;
}

void L_copy(sList *dst, sList *src, unsigned datasize) {
    assert(dst);
    assert(src);

    if(!src->count) return;

    bzero(dst, sizeof(sList));

    sLEl *curr;
    for(curr = src->head; curr; curr = curr->next)
        L_add_tail(dst, _memdup(curr->data, datasize));
}

void L_sort(sList *list, cmphandler hnd) {
    assert(list);
    assert(hnd);

    if(!list->count) return;

    sLEl *li, *lj, *next;

    for(lj = list->head->next; lj; ) {  // custom insertion sort
        for(li = lj; li->prev && hnd(li->prev->data, lj->data)>0; li = li->prev);

        next = lj->next;
        if(li != lj) {  // then move lj before li
            lj->prev->next = lj->next;
            if(lj->next) lj->next->prev = lj->prev;
            lj->prev = li->prev;
            lj->next = li;

            if(li->prev) li->prev->next = lj;
            li->prev = lj;
        }
        lj = next;
    }

    while(list->head->prev) list->head = list->head->prev;
    while(list->tail->next) list->tail = list->tail->next;
}

void L_filter(sList *list, resulthandler valid, datahandler del) {
    assert(list);
    assert(valid);

    if(!list->count) return;

    sLEl *curr = list->head, *next;
    while(curr) {
        next = curr->next;

        if(!valid(curr->data)) {
            if(del) del(curr->data);

            L_drop_el(list, curr);
        }

        curr = next;
    }
}

sLEl *L_find_el(sList *list, void *data) {
    assert(list);

    if(!list->count) return NULL;

    sLEl *curr;
    for(curr = list->head; curr; curr = curr->next)
        if(curr->data == data)
            return curr;

    return NULL;
}

void *L_drop_el(sList *list, sLEl *element) {
    assert(list);
    assert(list->count);
//    assert(L_find_el(list, element));

    if(!element) return NULL;

    if(element == list->head)
        list->head = element->next;

    if(element == list->tail)
        list->tail = element->prev;

    if(element->prev)
        element->prev->next = element->next;
    if(element->next)
        element->next->prev = element->prev;

    list->count--;

    void *data = element->data;

    free(element);

    return data;
}

inline void *L_drop(sList *list, void *data) {
    return L_drop_el(list, L_find_el(list, data));
}

void L_dropall(sList *list, datahandler hnd) {
    assert(list);

    if(!list->count) return;

    sLEl *curr = list->head, *next;
    while(curr) {
        next = curr->next;

        if(hnd) hnd(curr->data);
        free(curr);

        curr = next;
    }
}

