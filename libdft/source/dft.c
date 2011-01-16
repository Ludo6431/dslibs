#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef ARM9
#include <nds.h>
#endif

#include "dft.h"

#define _FMT(f) {#f, DFT_TEX_FMT_##f}
sTFmt DFT_TEX_FMTS[] = {
    _FMT(A3I5),
    _FMT(A0I2),
    _FMT(A0I4),
    _FMT(A0I8),
    _FMT(A5I3),
    _FMT(A1C15),
    {NULL}
};
#undef _FMT

unsigned dft_get_tex_fmt(char *str) {
    sTFmt *p = DFT_TEX_FMTS;
    while(p->name) {
        if(!strcasecmp(p->name, str))
            return p->fmt;
        p++;
    }
    return 0;
}

u16 dft_crc(char *data, unsigned size) {
    assert(data);

    #define CRCSEED 0xFFFF

#ifdef ARM9
    assert(!((unsigned)data&0x1));    // 2 bytes aligned
    assert(!(size&0x1));    // 2 bytes aligned

    return swiCRC16(CRCSEED, data, size);
#else

    u16 val[8] = { 0xC0C1, 0xC181, 0xC301, 0xC601, 0xCC01, 0xD801, 0xF001, 0xA001 };

    u16 crc = CRCSEED, carry;
    unsigned i, j;
    for(i=0; i<size; i++) {
        crc = crc^data[i];
        for(j=0; j<8; j++) {
            carry = crc&0x1;
            crc = crc>>1;
            if(carry)
                crc = crc^(val[j]<<(7-j));
        }
    }

    return crc;
#endif
}

int dft_check(void *data, unsigned datasize) {
    if(!data || datasize < sizeof(sDFT_HEADER))
        return 1;

    sDFT_HEADER *hdr = (sDFT_HEADER *)data;
    if(hdr->magic != DFT_HDR_MAGIC)
        return 1;

    unsigned eoffset = hdr->sections[DFT_EOF];
    if(datasize < eoffset + sizeof(sDFT_EOF))
        return 1;

    sDFT_EOF *eof = data + eoffset; 
    if(eof->magic != DFT_EOF_MAGIC)
        return 1;
    if(dft_crc(data, eoffset) != eof->crc)
        return 1;

    return 0;
}

#ifndef ARM9
#include <list.h>
#include "FT2dft.h" // TODO : that's a hacky way to get verbose, dump_texture, mexit and dofname symbols
#include "dump.h"

void dft_new_ranges(sList *ranges, sDFT_RANGES **dftranges) {
    assert(ranges);
    assert(dftranges);

    *dftranges = (sDFT_RANGES *)malloc(sizeof(sDFT_RANGES) + ranges->count*sizeof(sDFT_RANGE));
    if(!*dftranges) mexit(1, "ERR: not enough memory");

    (*dftranges)->num = ranges->count;

    sLEl *curr;
    unsigned i;
    for(curr = ranges->head,i=0; curr; curr = curr->next,i++)
        memcpy(&(*dftranges)->ranges[i], curr->data, sizeof(sDFT_RANGE));
}

void dft_new_map(sList *glyphs, sDFT_MAP **dftmap) {
    assert(glyphs);
    assert(dftmap);

    *dftmap = (sDFT_MAP *)malloc(sizeof(sDFT_MAP) + glyphs->count*sizeof(sDFT_GPROPS));
    if(!*dftmap) mexit(1, "ERR: not enough memory");

    (*dftmap)->num = glyphs->count;

    sLEl *curr;
    unsigned i;
    for(curr = glyphs->head,i=0; curr; curr = curr->next,i++)
        memcpy(&(*dftmap)->glyphs[i], curr->data, sizeof(sDFT_GPROPS));
}

void dft_new_texture(sList *glyphs, unsigned w, unsigned h, u32 format, sDFT_TEXTURE **dfttex, sDFT_PALETTE **dftpal) {
    assert(glyphs);
    assert(dfttex && dftpal);

    unsigned i, j, cu, cv, cw, ch;

    char *tex = (char *)calloc(1, w*h*3);
    if(!tex) mexit(1, "ERR: not enough memory");

if(verbose>2) { // fill with red the background
for(i=0; i<w*h*3; i+=3)
    tex[i]=255;
}

    // create grey scale texture
    sLEl *curr;
    for(curr = glyphs->head; curr; curr = curr->next) {
        cu = SGLYPH(curr)->props.u;
        cv = SGLYPH(curr)->props.v;
        cw = SGLYPH(curr)->props.w;
        ch = SGLYPH(curr)->props.h;

        if(cw*ch) {
            for(j=0; j<ch; j++) {
                for(i=0; i<cw; i++) {
                    tex[((cu + i) + (cv + j)*w)*3 + 0] = SGLYPH(curr)->bitmap[i + j*cw];    // R
                    tex[((cu + i) + (cv + j)*w)*3 + 1] = SGLYPH(curr)->bitmap[i + j*cw];    // G
                    tex[((cu + i) + (cv + j)*w)*3 + 2] = SGLYPH(curr)->bitmap[i + j*cw];    // B
                }
            }
        }
    }

    // dump texture
    dump_texture(dofname, tex, w, h);

// TODO : create fading palette when BG color is not transparent
    switch(format&DFT_TEX_FMTMASK) {
    case DFT_TEX_FMT_A3I5:
    case DFT_TEX_FMT_A5I3:
        (*dfttex) = (sDFT_TEXTURE *)malloc(sizeof(sDFT_TEXTURE) + w*h); // 1 byte per pixel
        if(!dfttex) mexit(1, "ERR: not enough memory");
        (*dftpal) = (sDFT_PALETTE *)calloc(1, sizeof(sDFT_PALETTE) + 2*1);  // 1 color
        if(!dftpal) mexit(1, "ERR: not enough memory");

        (*dfttex)->size = w*h;

        (*dftpal)->num = 1;
        (*dftpal)->data[0] = 0x7FFF; // white

        if(DFT_TEX_ISFMT(format, A5I3))
            for(i=0; i<w*h*3; i+=3)
                ((u8 *)(*dfttex)->data)[i/3] = ((255-tex[i])&0xF8);  // A5I3 (A = 5 bits white intensity; I = 0 (first pal color))
        else
            for(i=0; i<w*h*3; i+=3)
                ((u8 *)(*dfttex)->data)[i/3] = ((255-tex[i])&0xE0);  // A5I3 (A = 3 bits white intensity; I = 0 (first pal color))
        break;
    default:
        mexit(1, "ERR: texture format not yet implemented");
    }

    free(tex);

    (*dfttex)->fmt = format;
    (*dfttex)->w = w;
    (*dfttex)->h = h;
}

void dft_write(char *ofname, sDFT_RANGES *ranges, sDFT_MAP *map, sDFT_TEXTURE *tex, sDFT_PALETTE *pal) {
    assert(ofname);
    assert(ranges && map && tex && pal);

    FILE *ofd = fopen(ofname, "wb+");
    if(!ofd) mexit(1, "Can't create/reset file \"%s\"", ofname);

    unsigned size = 0, tsize;

    // prepare header
    sDFT_HEADER hdr;
    RESET(&hdr, sDFT_HEADER);
    hdr.magic = DFT_HDR_MAGIC;
    hdr.numsections = DFT_NUM_SECTIONS;
    fseek(ofd, 0, SEEK_SET);
    tsize = sizeof(sDFT_HEADER);
    size += ALIGN(tsize, 4);

    // save ranges
    hdr.sections[DFT_RANGES] = size;
    fseek(ofd, size, SEEK_SET);
    tsize = sizeof(sDFT_RANGES) + ranges->num*sizeof(sDFT_RANGE);
    fwrite((void *)ranges, 1, tsize, ofd);
    size += ALIGN(tsize, 4);

    // save map
    hdr.sections[DFT_MAP] = size;
    fseek(ofd, size, SEEK_SET);
    tsize = sizeof(sDFT_MAP) + map->num*sizeof(sDFT_GPROPS);
    fwrite((void *)map, 1, tsize, ofd);
    size += ALIGN(tsize, 4);

    // save texture
    hdr.sections[DFT_TEXTURE] = size;
    fseek(ofd, size, SEEK_SET);
    tsize = sizeof(sDFT_TEXTURE) + tex->size;
    fwrite((void *)tex, 1, tsize, ofd);
    size += ALIGN(tsize, 4);

    // save palette
    hdr.sections[DFT_PALETTE] = size;
    fseek(ofd, size, SEEK_SET);
    tsize = sizeof(sDFT_PALETTE) + pal->num*2;
    fwrite((void *)pal, 1, tsize, ofd);
    size += ALIGN(tsize, 4);

    // save EOF
    hdr.sections[DFT_EOF] = size;
        // save header
        fseek(ofd, 0, SEEK_SET);
        fwrite((void *)&hdr, 1, sizeof(sDFT_HEADER), ofd);
    sDFT_EOF eof;
    RESET(&eof, sDFT_EOF);
    eof.magic = DFT_EOF_MAGIC;
    {   // calculate crc
        char *data = (char *)malloc(size);
        if(!data) mexit(1, "ERR: not enough memory");
        fseek(ofd, 0, SEEK_SET);
        fread(data, 1, size, ofd);
        eof.crc = dft_crc(data, size);
        free(data);
    }
    fseek(ofd, size, SEEK_SET);
    tsize = sizeof(sDFT_EOF);
    fwrite((void *)&eof, 1, tsize, ofd);
    size += tsize;

    fclose(ofd);

if(verbose)
printf("dft filesize = %d\n", size);
if(verbose)
printf("dft crc = %02X\n", eof.crc);
}

void dft_cleanup(sDFT_RANGES *ranges, sDFT_MAP *map, sDFT_TEXTURE *tex, sDFT_PALETTE *pal) {
    if(ranges) free(ranges);
    if(map) free(map);
    if(tex) free(tex);
    if(pal) free(pal);    
}

#endif

