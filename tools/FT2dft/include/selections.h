#ifndef _SELECTIONS_H
#define _SELECTIONS_H

typedef struct Selection sSelection;
struct Selection {
    unsigned start;
    unsigned end;   // if !end go to the end
};

int sel_add         (sSelection *selects, unsigned *num_selects, char *str);
int sel_contains    (sSelection *selects, unsigned num_selects, unsigned charcode);

#endif

