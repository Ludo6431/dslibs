#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifdef ARM9
#include <nds.h>
#endif

#include "dft.h"

#ifdef ARM9
#   define _FMT(f, dsf) {#f, DFT_TEX_FMT_##f, GL_##dsf}
#else
#   define _FMT(f, dsf) {#f, DFT_TEX_FMT_##f, 0}
#endif
sTFmt DFT_TEX_FMTS[] = {
    _FMT(A0I2,  RGB4),
    _FMT(A5I3,  RGB8_A5),
    _FMT(A0I4,  RGB16),
    _FMT(A3I5,  RGB32_A3),
    _FMT(A0I8,  RGB256),
    _FMT(A1C15, RGBA),
    {NULL}
};
#undef _FMT

unsigned dft_get_tex_fmt(char *str) {
    assert(str);

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

    assert(!((unsigned)data&0x1));    // 2 bytes aligned
    assert(!(size&0x1));    // 2 bytes aligned

#ifdef ARM9
    return swiCRC16(CRCSEED, data, size);
#else
    // gbatek is wrong, check : http://desmume.svn.sourceforge.net/viewvc/desmume/trunk/desmume/src/bios.cpp?revision=3877&content-type=text%2Fplain (line 1018)
    const u16 val[] = { 0x0000,0xCC01,0xD801,0x1400,0xF001,0x3C00,0x2800,0xE401,0xA001,0x6C00,0x7800,0xB401,0x5000,0x9C01,0x8801,0x4400};
    u16 crc = CRCSEED;
    u32 i, j;

    for(i=0; i<size>>1; i++) {
        u16 currVal = ((u16 *)data)[i];

        for(j=0; j<4; j++) {
            u16 tabVal = val[crc&0xF];
            crc >>= 4;
            crc ^= tabVal;

            u16 tempVal = currVal >> (4*j);
            tabVal = val[tempVal&0xF];
            crc ^= tabVal;
        }
    }

    return crc;
#endif

    #undef CRCSEED
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

int dft_init(sDFT *font, char *fname) {
    assert(font);
    assert(fname);

    bzero(font, sizeof(sDFT));

    FILE *fd = NULL;
    char *data = NULL;
    unsigned datasize;

    fd = fopen(fname, "rb+");
    if(!fd)
        goto fail;

    fseek(fd, 0, SEEK_END);
    datasize = ftell(fd);
    rewind(fd);

    data = malloc(datasize);
    if(!data)
        goto fail;
    if(fread(data, 1, datasize, fd) != datasize)
        goto fail;

    if(dft_check(data, datasize))
        goto fail;

    font->data = data;
    font->datasize = datasize;

    fclose(fd);

    return 0;

fail:
    if(data) free(data);
    if(fd) fclose(fd);

    return 1;
}

sDFT_GPROPS *dft_get_glyph(sDFT *font, u32 c) {
    assert(font);
    assert(font->data);

    sDFT_RANGE *curr = DFT_SECTION(font->data, DFT_RANGES)->ranges;
    unsigned num = DFT_SECTION(font->data, DFT_RANGES)->num;
    while(num--) {
        if(curr->first_car <= c && c <= curr->last_car)
            return &DFT_SECTION(font->data, DFT_MAP)->glyphs[curr->map_offset + c - curr->first_car];

        curr++;
    }

    return NULL;
}

#ifdef ARM9
unsigned dft_get_tex_dsfmt(char *str, unsigned fmt) {   // if str use str else use fmt
    sTFmt *p = DFT_TEX_FMTS;
    if(str)
        while(p->name) {
            if(!strcasecmp(p->name, str))
                return p->dsfmt;
            p++;
        }
    else
        while(p->name) {
            if(p->fmt == (fmt&DFT_TEX_FMTMASK))
                return p->dsfmt;
            p++;
        }
    return 0;
}

unsigned dft_get_tex_dssize(unsigned sz) {
    if(sz<=8)       return TEXTURE_SIZE_8;
    if(sz<=16)      return TEXTURE_SIZE_16;
    if(sz<=32)      return TEXTURE_SIZE_32;
    if(sz<=64)      return TEXTURE_SIZE_64;
    if(sz<=128)     return TEXTURE_SIZE_128;
    if(sz<=256)     return TEXTURE_SIZE_256;
    if(sz<=512)     return TEXTURE_SIZE_512;
    if(sz<=1024)    return TEXTURE_SIZE_1024;

    return 0;
}

unsigned dft_get_tex_dsparam(sDFT_TEXTURE *tex) {
    assert(tex);

    unsigned param = 0 | TEXGEN_OFF;

    if((tex->fmt & DFT_TEX_HASPALETTE) && (tex->fmt & DFT_TEX_PAL1TRANS))
        param |= GL_TEXTURE_COLOR0_TRANSPARENT;

    return param;
}

int dft_load(sDFT *font) {
    assert(font);

    if(!font->data) return 0;

    // load texture
    sDFT_TEXTURE *ftex = DFT_SECTION(font->data, DFT_TEXTURE);
    font->texFmt = dft_get_tex_dsfmt(NULL, ftex->fmt); // get texture ds format with dft format
    glGenTextures(1, &font->texId);
    glBindTexture(0, font->texId);
    glTexImage2D(
        0, 0,   /* useless / compatibility */
        font->texFmt /* type */,
        dft_get_tex_dssize(ftex->w) /* X size */,
        dft_get_tex_dssize(ftex->h) /* Y size */,
        0,  /* useless / compatibility */
        dft_get_tex_dsparam(ftex) /* texture params */,
        ftex->data
    );

    // load texture palette
    sDFT_PALETTE *fpal = DFT_SECTION(font->data, DFT_PALETTE);
    font->palAddr = gluTexLoadPal(fpal->data, fpal->num&1?fpal->num+1:fpal->num /* XXX : libnds fix pending */, font->texFmt);

    return font->texId;
}

#else /* ifdef ARM9 */

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
    u8 carmask = 0;
    switch(format&DFT_TEX_FMTMASK) {
    case DFT_TEX_FMT_A3I5:
        carmask = 0xE0;
    case DFT_TEX_FMT_A5I3:
        if(!carmask) carmask = 0xF8;

        (*dfttex) = (sDFT_TEXTURE *)malloc(sizeof(sDFT_TEXTURE) + w*h); // 1 byte per pixel
        if(!dfttex) mexit(1, "ERR: not enough memory");
        (*dftpal) = (sDFT_PALETTE *)calloc(1, sizeof(sDFT_PALETTE) + 2*1);  // 1 color
        if(!dftpal) mexit(1, "ERR: not enough memory");

        (*dfttex)->size = w*h;

        (*dftpal)->num = 1;
        (*dftpal)->data[0] = 0x7FFF; // white

        for(i=0; i<w*h; i++)
            ((u8 *)(*dfttex)->data)[i] = tex[3*i]&carmask;//((255-tex[3*i])&carmask);

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
