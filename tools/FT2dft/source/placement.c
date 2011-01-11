#include <stdlib.h>
#include <stdio.h>
#include "FT2dft.h"

#include "tools.h"
#include "limitline.h"

#include "placement.h"

#if 0
sLEl *chooseglyph(sList *list, unsigned dw, unsigned dh) {
    sLEl *curr;

    if(dh) {
        for(curr = list->head; curr; curr = curr->next)
            if((!dw || SGLYPH(curr)->props.w <= dw) && SGLYPH(curr)->props.h <= dh)
                break;
    }
    else {
        sLEl *choice = NULL;

        for(curr = list->head; curr; curr = curr->next) {
            if(!choice && SGLYPH(curr)->props.w <= dw)
                choice = curr;

            if(SGLYPH(curr)->props.w == dw)
                break;
        }

        if(!curr) curr = choice;
    }

    return curr;
}

void genplacement(sList *glyphs, unsigned width, unsigned *height) {
int verbose = 1;
    sLEl *curr, *next;
    sList glcopy;
    RESET(&glcopy, sList);

    // init the sorted copy
    L_copy(&glcopy, glyphs, 0);    // don't copy the data, just copy the pointer (see _memdup(*,*,0) in list.c)

    int _glyphvalid(sDFT_GLYPH *g) {
        return g->props.w * g->props.h;
    }
    L_filter(&glcopy, (resulthandler)_glyphvalid, NULL /* don't free the data */);   // don't keep the caracters without surface

    int _glyphcmp(sDFT_GLYPH *a, sDFT_GLYPH *b) { // sort first by height and then by width
        int diff;

        diff = a->props.h - b->props.h;
        if(diff) return -diff;

        diff = a->props.w - b->props.w;
        return -diff;
    }
    L_sort(&glcopy, (cmphandler)_glyphcmp);

/*    for(curr = glcopy.head; curr; curr = curr->next)*/
/*        printf("%08X (%dx%d)\n", SGLYPH(curr)->ccode, SGLYPH(curr)->props.w, SGLYPH(curr)->props.h);*/

    // create the dynamic breakable line representing the vertical down limit of the already placed caracters
    sList limit;
    RESET(&limit, sList);
    sLimit *lim = NEW(sLimit);
    lim->start = 0;
    lim->end = width-1;
    lim->height = 0;
    L_add_tail(&limit, lim);

    // let's go, optimize the texture !
    unsigned dh, dw, u, v;
    sLEl *curlim, *nextlim;
    do {
if(verbose) {
lim_print(&limit, "glob");
printf("%d caracters\n", glcopy.count);
}

        curlim = limit.head;
        while(curlim) {
            if(!glcopy.count) break;    // we're done !

if(verbose)
printf("> curlim = "LIMFMT" : ", LIMARGS(curlim));
            // get desired size of next caracter
            dh = curlim->prev ? (LIMITH(curlim->prev)-LIMITH(curlim)) : 0;
            dw = LIMITW(curlim);

if(verbose)
printf("dsize = %03dx%03d => ", dw, dh);

            // choose next caracter based on desired size
            curr = chooseglyph(&glcopy, dw, dh);
            if(!curr) { // TODO (and fill u & v)
                if(curlim->next) {
                    if(LIMITH(curlim->next) < LIMITH(curlim)) {
                        // might be worth the shoot
if(verbose)
printf("[suivant plus bas]");

                        sLEl *try = chooseglyph(&glcopy, dw, 0);
                        if(try) {
if(verbose)
printf("!");
                            curr = try;
                        }
                        else {
if(verbose)
printf("continue\n");
                            curlim = curlim->next;
                            continue;
                        }
                    }
                    else {
if(verbose)
printf("[suivant plus haut]");
                        sLEl *next = curlim->next;
                        SLIMIT(curlim)->height = SLIMIT(curlim->next)->height;
                        lim_group(&limit);

if(verbose)
printf("continue\n");
                        curlim = next;
                        continue;
                    }
                }
                else {
                    // dh = 0;
                    // don't put a caracter with bigger height
if(verbose)
printf("[pas de suivant]");
                }
            }

            if(!curr) {
if(verbose)
printf("break\n");
                break;
            }

if(verbose)
printf("glyph : %08x (%dx%d)\n", SGLYPH(curr)->ccode,SGLYPH(curr)->props.w, SGLYPH(curr)->props.h);

            // update glyph coordinates
            sDFT_GLYPH *gl = L_drop_el(&glcopy, curr);
            gl->props.u = SLIMIT(curlim)->start;
            gl->props.v = SLIMIT(curlim)->height;

            // update limit line and get next segment in curlim
            lim_addglyph_left(&limit, &curlim, gl);
        }
    } while(glcopy.count);
lim_print(&limit, "end");

    if(height) {
        unsigned h=0;
        for(curlim = limit.head; curlim; curlim = curlim->next)
            if(SLIMIT(curlim)->height > h)
                h = SLIMIT(curlim)->height;

        *height = h;
    }

    // let's free some mem
    L_dropall(&glcopy, NULL);
    L_dropall(&limit, free);
}

#else

sLEl *chooseglyph(sList *list, unsigned dh_l, unsigned dw, unsigned dh_r, int *place) {
    sLEl *curr; // TODO

#ifndef MIN
#   define MIN(a, b) ((a)>(b) ? (b) : (a))
#endif

#ifndef MAX
#   define MAX(a, b) ((a)>(b) ? (a) : (b))
#endif

    unsigned maxdh = MAX(dh_l, dh_r);
    unsigned mindh = MIN(dh_l, dh_r);

    if(maxdh) {
        for(curr = list->head; curr; curr = curr->next)
            if((!dw || SGLYPH(curr)->props.w <= dw) && SGLYPH(curr)->props.h == maxdh)
                break;

        if(curr) {
            if(place)
                *place = (maxdh == dh_l)?-1:1;
        }
        else if(mindh) {
            for(curr = list->head; curr; curr = curr->next)
                if((!dw || SGLYPH(curr)->props.w <= dw) && SGLYPH(curr)->props.h == mindh)
                    break;

            if(curr) {
                if(place)
                    *place = (mindh == dh_l)?-1:1;
            }
        }
    }
    else {
        sLEl *choice = NULL;

        for(curr = list->head; curr; curr = curr->next) {
            if(!choice && SGLYPH(curr)->props.w <= dw)
                choice = curr;

            if(SGLYPH(curr)->props.w == dw)
                break;
        }

        if(!curr) {
            if(place) *place = -1;
            curr = choice;
        }
    }

    return curr;
}

void genplacement(sList *glyphs, unsigned width, unsigned *height) {
    sLEl *curr, *next;
    sList glcopy;
    RESET(&glcopy, sList);

    // init the sorted copy
    L_copy(&glcopy, glyphs, 0);    // don't copy the data, just copy the pointer (see _memdup(*,*,0) in list.c)

    int _glyphvalid(sDFT_GLYPH *g) {
        return g->props.w * g->props.h;
    }
    L_filter(&glcopy, (resulthandler)_glyphvalid, NULL /* don't free the data */);   // don't keep the caracters without surface

    int _glyphcmp(sDFT_GLYPH *a, sDFT_GLYPH *b) { // sort first by height and then by width
        int diff;

        diff = a->props.h - b->props.h;
        if(diff) return -diff;

        diff = a->props.w - b->props.w;
        return -diff;
    }
    L_sort(&glcopy, (cmphandler)_glyphcmp);

/*    for(curr = glcopy.head; curr; curr = curr->next)*/
/*        printf("%08X (%dx%d)\n", SGLYPH(curr)->ccode, SGLYPH(curr)->props.w, SGLYPH(curr)->props.h);*/

    // create the dynamic breakable line representing the vertical down limit of the already placed caracters
    sList limit;
    RESET(&limit, sList);
    sLimit *lim = NEW(sLimit);
    lim->start = 0;
    lim->end = width-1;
    lim->height = 0;
    L_add_tail(&limit, lim);

    // let's go, optimize the texture !
    unsigned dh_l, dh_r, dw, u, v;
    int place = 0;
    sLEl *curlim, *nextlim;
    do {
if(verbose) {
lim_print(&limit, "glob");
printf("%d caracters left\n", glcopy.count);
}

        curlim = lim_get_min_el(&limit);

if(verbose)
printf("> curlim = \'"LIMFMT"\' : ", LIMARGS(curlim));
        // get desired size of next caracter
        dh_l = curlim->prev ? (LIMITH(curlim->prev)-LIMITH(curlim)) : 0;
        dh_r = curlim->next ? (LIMITH(curlim->next)-LIMITH(curlim)) : 0;
        dw = LIMITW(curlim);

if(verbose)
printf("dsize = %03ux%03ux%03u => ", dh_l, dw, dh_r);

        // choose next caracter based on desired size
        curr = chooseglyph(&glcopy, dh_l, dw, dh_r, &place);
        if(!curr) {
            sLEl *try = chooseglyph(&glcopy, 0, dw, 0, &place); // try without height limit
            if(try) {
if(verbose)
printf("!");
                curr = try;
            }
            else {  // no solution for that limit => remove that limit and merge it with the prev or next limit
if(verbose)
printf("remove lim \'"LIMFMT"\' (%d -> ", LIMARGS(curlim), LIMITH(curlim));
                if(curlim->prev)
                    SLIMIT(curlim)->height = SLIMIT(curlim->prev)->height;
                else if(curlim->next)
                    SLIMIT(curlim)->height = SLIMIT(curlim->next)->height;
                else
                    printf("omfgwtf?\n");
if(verbose)
printf("%d)\n", LIMITH(curlim));
                lim_group(&limit);

                continue;
            }
        }

        if(!curr) {
if(verbose)
printf("break\n");
            break;
        }

if(verbose)
printf("glyph : %08x (%dx%d)\n", SGLYPH(curr)->ccode,SGLYPH(curr)->props.w, SGLYPH(curr)->props.h);

        // update glyph coordinates

        // update limit line and get next segment in curlim
        sDFT_GLYPH *gl = L_drop_el(&glcopy, curr);
        if(place < 0) {
            gl->props.u = SLIMIT(curlim)->start;
            gl->props.v = SLIMIT(curlim)->height;

            lim_addglyph_left(&limit, &curlim, gl);
        }
        else {
            gl->props.u = SLIMIT(curlim)->end - gl->props.w + 1;
            gl->props.v = SLIMIT(curlim)->height;

            lim_addglyph_right(&limit, &curlim, gl);
        }

    } while(glcopy.count);
lim_print(&limit, "end");

    if(height) {
        unsigned h=0;
        for(curlim = limit.head; curlim; curlim = curlim->next)
            if(SLIMIT(curlim)->height > h)
                h = SLIMIT(curlim)->height;

        *height = h;
    }

    // let's free some mem
    L_dropall(&glcopy, NULL);
    L_dropall(&limit, free);
}

#endif
