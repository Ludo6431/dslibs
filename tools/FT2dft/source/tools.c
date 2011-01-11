#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "FT2dft.h"

#include "tools.h"

void add_range(sList *ranges, unsigned start, unsigned end) {
    static offset = 0;

    sDFT_RANGE *range = NEWZ(sDFT_RANGE);
    assert(range);

    range->map_offset = offset;
    range->first_car = start;
    range->last_car = end;

    L_add_tail(ranges, range);

    offset += end-start+1;
}

void add_glyph(sList *glyphs, FT_GlyphSlot fglyph, unsigned ccode) {
    sDFT_GLYPH *glyph = NEWZ(sDFT_GLYPH);
    assert(glyph);

    glyph->ccode = ccode;
    glyph->props.w = fglyph->bitmap.width;
    glyph->props.h = fglyph->bitmap.rows;
    glyph->props.dx = fglyph->bitmap_left;
    glyph->props.dy = fglyph->bitmap_top;
    glyph->props.adx = fglyph->advance.x >> 6;
    if(glyph->props.w * glyph->props.h) {
        glyph->bitmap = (u8 *)malloc(glyph->props.w * glyph->props.h);
        assert(glyph->bitmap);
        memcpy(glyph->bitmap, fglyph->bitmap.buffer, glyph->props.w * glyph->props.h);
    }

    L_add_tail(glyphs, glyph);
}

char *add_suffix(char *base, char *suffix) {
    #define LENMAX 1024

    if(strlen(base)>1024) return NULL;

    static char new[LENMAX];

    strcpy(new, base);
    strcat(new, suffix);

    return new;
}


