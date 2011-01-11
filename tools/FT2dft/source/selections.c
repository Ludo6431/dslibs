#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "selections.h"

int sel_add(sSelection *selects, unsigned *num_selects, char *str) {    // TODO parse a:z | 20:0x30 | ...
    if(!selects || !num_selects || !str) return 1;

    unsigned start, end;

    switch(sscanf(str, "%x:%x", &start, &end)) {
    case 0: // parsed nothing
        start = 0;

        switch(sscanf(str, ":%x", &end)) {   // try to parse end
        case 1: // parsed end
            break;
        default:
            return 1;
        }

        break;
    case 1: // parsed only start
        end = 0;
    case 2: // parsed start and end
        break;
    default:
        return 1;
        break;
    }

    if(end && end < start)
        return 1;

    selects[*num_selects].start = start;
    selects[*num_selects].end = end;
    (*num_selects)++;

    return 0;
}

int sel_contains(sSelection *selects, unsigned num_selects, unsigned charcode) {
    if(!selects || !num_selects) return 0;

    unsigned i;

    for(i=0; i<num_selects; i++) {
        if(selects[i].end) {
            if(selects[i].start <= charcode && charcode <= selects[i].end)
                return 1;
        }
        else {
            if(selects[i].start <= charcode)
                return 1;
        }
    }

    return 0;
}

