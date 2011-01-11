#ifndef _FTREAD_H
#define _FTREAD_H

#include "list.h"
#include "selections.h"

void FTread(char *ifname, unsigned ofntsize, sSelection *selects, unsigned num_selects, sList *glyphs, sList *ranges, unsigned *_bmpsurface);

#endif

