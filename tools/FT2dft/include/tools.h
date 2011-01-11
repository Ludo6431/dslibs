#ifndef _FT2DFTTOOLS_H
#define _FT2DFTTOOLS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "list.h"
#include "dft.h"

#define SGLYPH(el)  ((sDFT_GLYPH *)(((sLEl *)(el))->data))
#define SRANGE(el)  ((sDFT_RANGE *)(((sLEl *)(el))->data))

void    add_range   (sList *ranges, unsigned start, unsigned end);
void    add_glyph   (sList *glyphs, FT_GlyphSlot fglyph, unsigned ccode);

char *  add_suffix  (char *base, char *suffix);

#endif

