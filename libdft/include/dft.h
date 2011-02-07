#ifndef _DFT_H
#define _DFT_H

#ifndef ARM9  // When not building for NDS
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
#endif

#ifndef PACKED
#   define PACKED __attribute__((__packed__))
#endif

typedef struct DFT sDFT;
struct DFT {
    char *data;
    unsigned datasize;
    unsigned texFmt;
    int texId;
    int palAddr;
};

#define DFT_SECTION(dat, sec) ((s##sec *)( (dat)+((sDFT_HEADER *)(dat))->sections[sec] ))

// "DSFT" if LE
#define DFT_HDR_MAGIC ( 'D' + ('S' << 8) + ('F' << 16) + ('T' << 24) )

enum {
    DFT_EOF,
    DFT_RANGES,
    DFT_MAP,
    DFT_TEXTURE,
    DFT_PALETTE,

    DFT_NUM_SECTIONS
};

typedef struct DFT_HEADER sDFT_HEADER;
struct DFT_HEADER {
    u32 magic;      // "DSFT" LE
    u32 numsections;
    u32 sections[DFT_NUM_SECTIONS];  // each section should be 4-bytes aligned
};

#define DFT_EOF_MAGIC ( 0xDE + (0xAD << 8) )

typedef struct DFT_EOF sDFT_EOF;
struct DFT_EOF {
    u16 crc;    // crc16 (DS bios function) of whole file until hdr.offsets[DFT_SEC_EOF]
    u16 magic;  // 0xADDE LE
};

// not used in dft files
typedef struct DFT_RANGE sDFT_RANGE;
struct DFT_RANGE {
    u32 map_offset;
    u32 first_car;
    u32 last_car;
} PACKED;

typedef struct DFT_RANGES sDFT_RANGES;
struct DFT_RANGES {
    u32 num;
    sDFT_RANGE ranges[];
};

typedef struct DFT_GPROPS sDFT_GPROPS;
struct DFT_GPROPS {
    u16 u:12;
    u16 v:12;
    u8 adx;
    u8 w, h;
    s8 dx, dy;
} PACKED;

// not used in dsf files
typedef struct DFT_GLYPH sDFT_GLYPH;
struct DFT_GLYPH {
    sDFT_GPROPS props;
    u8 *bitmap;
    unsigned ccode;
};

typedef struct DFT_MAP sDFT_MAP;
struct DFT_MAP {
    u32 num;
    sDFT_GPROPS glyphs[];
};

#define DFT_TEX_HASPALETTE  (          1 <<  0 )    // 1 bit    : has a palette
#define DFT_TEX_PAL1TRANS   (          1 <<  1 )    // 1 bit    : first color of palette must be transparent
#define DFT_TEX_PIXELBIT(n) ( ((n)&0x1F) <<  2 )    // 5 bits   : number of bits by pixel
#define DFT_TEX_ALPHABIT(n) ( ((n)&0x07) <<  7 )    // 3 bits   : number of bits coding the alpha channel
#define DFT_TEX_COLORBIT(n) ( ((n)&0x0F) << 10 )    // 4 bits   : number of bits coding the color (or index in palette if any)

#define DFT_TEX_GETPIXELBIT(n) ( ((n) >> 2)&0x1F )
#define DFT_TEX_GETALPHABIT(n) ( ((n) >> 7)&0x07 )
#define DFT_TEX_GETCOLORBIT(n) ( ((n) >> 10)&0x0F )

#define DFT_TEX_FMTMASK     ( 1 | (0x1F << 2) | (0x7 << 7) | (0xF << 10) )
#define DFT_TEX_ISFMT(f, t) ( ((f)&DFT_TEX_FMTMASK) == DFT_TEX_FMT_##t )

#define DFT_TEX_FMT_A0I2    ( DFT_TEX_HASPALETTE | DFT_TEX_PIXELBIT( 2) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 2) )
#define DFT_TEX_FMT_A5I3    ( DFT_TEX_HASPALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 5) | DFT_TEX_COLORBIT( 3) )
#define DFT_TEX_FMT_A0I4    ( DFT_TEX_HASPALETTE | DFT_TEX_PIXELBIT( 4) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 4) )
#define DFT_TEX_FMT_A3I5    ( DFT_TEX_HASPALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 3) | DFT_TEX_COLORBIT( 5) )
#define DFT_TEX_FMT_A0I8    ( DFT_TEX_HASPALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 8) )
#define DFT_TEX_FMT_A1C15   (                      DFT_TEX_PIXELBIT(16) | DFT_TEX_ALPHABIT( 1) | DFT_TEX_COLORBIT(15) )

typedef struct {
    char *name;
    u32 fmt;
    u32 dsfmt;
} sTFmt;
extern sTFmt DFT_TEX_FMTS[];

typedef struct DFT_TEXTURE sDFT_TEXTURE;
struct DFT_TEXTURE {
    u32 fmt;    // format
    u16 w, h;   // width and height
    u32 size;
    u8 data[];  // texture data
};

typedef struct DFT_PALETTE sDFT_PALETTE;
struct DFT_PALETTE {
    u32 num;
    u16 data[];  // palette data
};

unsigned        dft_get_tex_fmt     (char *str);
u16             dft_crc             (char *data, unsigned size);
int             dft_check           (void *data, unsigned datasize);
int             dft_init            (sDFT *font, char *fname);
sDFT_GPROPS *   dft_get_glyph       (sDFT *font, u32 c);
#ifdef ARM9
unsigned        dft_get_tex_dsfmt   (char *str, unsigned fmt);
unsigned        dft_get_tex_dssize  (unsigned sz);
unsigned        dft_get_tex_dsparam (sDFT_TEXTURE *tex);
int             dft_load            (sDFT *font);
#else
#include "list.h"
void            dft_new_ranges      (sList *ranges, sDFT_RANGES **dftranges);
void            dft_new_map         (sList *glyphs, sDFT_MAP **dftmap);
void            dft_new_texture     (sList *glyphs, unsigned w, unsigned h, u32 format, sDFT_TEXTURE **dfttex, sDFT_PALETTE **dftpal);
void            dft_write           (char *ofname, sDFT_RANGES *ranges, sDFT_MAP *map, sDFT_TEXTURE *tex, sDFT_PALETTE *pal);
void            dft_cleanup         (sDFT_RANGES *ranges, sDFT_MAP *map, sDFT_TEXTURE *tex, sDFT_PALETTE *pal);
#endif

#endif

