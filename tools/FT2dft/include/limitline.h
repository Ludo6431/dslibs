#ifndef _LIMITLINE_H
#define _LIMITLINE_H

#define SLIMIT(el)      ((sLimit *)(((sLEl *)(el))->data))
#define LIMITW(el)    (SLIMIT(el)->end - SLIMIT(el)->start + 1)
#define LIMITH(el)    (SLIMIT(el)->height)

#define LIMFMT      "%03u->%03u (%03u) @ %03u"
#define LIMARGS(l)     SLIMIT(l)->start, SLIMIT(l)->end, LIMITW(l), SLIMIT(l)->height

typedef struct sLimit sLimit;
struct sLimit {
    unsigned start;
    unsigned end;
    unsigned height;
};

void    lim_print           (sList *limit, char *header);
void    lim_group           (sList *limit);
sLEl *  lim_get_min_el      (sList *limit);
sLEl *  lim_get_max_el      (sList *limit);
void    lim_addglyph_left   (sList *limit, sLEl *curlim, sDFT_GLYPH *glyph);
void    lim_addglyph_right  (sList *limit, sLEl *curlim, sDFT_GLYPH *glyph);

#endif

