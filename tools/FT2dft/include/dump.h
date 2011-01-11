#ifndef _DUMP_H
#define _DUMP_H

#include "list.h"

void    dump_glyphs_size        (char *dofname, sList *glyphs);
void    dump_glyphs_positions   (char *dofname, sList *glyphs);
void    dump_texture            (char *dofname, char *tex, unsigned w, unsigned h);

#endif

