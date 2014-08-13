#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "FT2dft.h"

#include "list.h"
#include "dft.h"

#include "limitline.h"

void lim_print(sList *limit, char *header) {
    assert(limit);

    printf("--- limit(%02d) --- <= %s\n", limit->count, header);

    sLEl *curlim;
    for(curlim = limit->head; curlim; curlim = curlim->next)
        printf(LIMFMT"\n", LIMARGS(curlim));
}

void lim_group(sList *limit) {
    assert(limit); assert(limit->count);

    if(limit->count < 2) return;

    sLEl *curr = limit->head, *next;

    while(curr) {
        next = curr->next;

        if(!LIMITW(curr)) {
            free(L_drop_el(limit, curr));
        }
        else if(curr->prev && SLIMIT(curr->prev)->height == SLIMIT(curr)->height) {
                SLIMIT(curr)->start = SLIMIT(curr->prev)->start;

                free(L_drop_el(limit, curr->prev));
        }

        curr = next;
    }
}

sLEl *lim_get_min_el(sList *limit) {
    assert(limit);

    if(!limit->count) return NULL;

    unsigned min = 0;
    sLEl *curr, *elmin = NULL;

    for(curr = limit->head; curr; curr = curr->next)
        if(!elmin || SLIMIT(curr)->height < min) {
            elmin = curr;
            min = SLIMIT(curr)->height;
        }

    return elmin;
}

sLEl *lim_get_max_el(sList *limit) {
    assert(limit);

    if(!limit->count) return NULL;

    unsigned max = 0;
    sLEl *curr, *elmax = NULL;

    for(curr = limit->head; curr; curr = curr->next)
        if(!elmax || SLIMIT(curr)->height > max) {
            elmax = curr;
            max = SLIMIT(curr)->height;
        }

    return elmax;
}

void lim_addglyph_left(sList *limit, sLEl *curlim, sDFT_GLYPH *glyph) {
    assert(limit); assert(curlim); assert(glyph);

    sLimit *lim = NEW(sLimit);
    lim->start = SLIMIT(curlim)->start;
    lim->end = lim->start + glyph->props.w - 1;
    lim->height = SLIMIT(curlim)->height + glyph->props.h;

    L_insert_after_el(limit, curlim->prev, lim);

    SLIMIT(curlim)->start += glyph->props.w;
}

void lim_addglyph_right(sList *limit, sLEl *curlim, sDFT_GLYPH *glyph) {
    assert(limit); assert(curlim); assert(glyph);

    sLimit *lim = NEW(sLimit);
    lim->start = SLIMIT(curlim)->end - glyph->props.w + 1;
    lim->end = SLIMIT(curlim)->end;
    lim->height = SLIMIT(curlim)->height + glyph->props.h;

    L_insert_after_el(limit, curlim, lim);

    SLIMIT(curlim)->end -= glyph->props.w;
}
