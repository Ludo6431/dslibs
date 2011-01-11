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

#define DFT_HDR_MAGIC ( 'D' + ('S' << 8) + ('F' << 16) + ('T' << 24) )

#define DFT_EOF     0
#define DFT_TEXTURE 1
#define DFT_PALETTE 2
#define DFT_RANGES  3
#define DFT_MAP     4

typedef struct sDFT_HEADER sDFT_HEADER;
struct sDFT_HEADER {
    u32 magic;      // 'DSFT'
    u32 num;
    u32 sections[];  // each section should be 4-bytes aligned
};

#define DFT_EOF_MAGIC ( '\xDE' + ('\xAD' << 8) )

typedef struct sDFT_EOF sDFT_EOF;
struct sDFT_EOF {
    u16 crc;    // crc16 (DS bios function) of whole file until hdr.offsets[DFT_SEC_EOF]
    u16 magic;  // 0xDEAD
};

#define DFT_TEX_HAS_PALETTE (          1 <<  0 )    // 1 bit    : has a palette
#define DFT_TEX_PAL1TRANS   (          1 <<  1 )    // 1 bit    : first color of palette must be transparent
#define DFT_TEX_PIXELBIT(n) ( ((n)&0x1F) <<  2 )    // 5 bits   : number of bits by pixel
#define DFT_TEX_ALPHABIT(n) ( ((n)&0x07) <<  7 )    // 3 bits   : number of bits coding the alpha channel
#define DFT_TEX_COLORBIT(n) ( ((n)&0x0F) << 10 )    // 4 bits   : number of bits coding the color (or index in palette if any)

#define DFT_TEX_FMT_MASK    ( 1 | (0x1F << 2) | (0x7 << 7) | (0xF << 10) )
#define DFT_TEX_ISFMT(f, t) ( (f)&DFT_TEX_FMT_MASK == DFT_TEX_FMT_##t )

#define DFT_TEX_FMT_A3I5    ( DFT_TEX_HAS_PALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 3) | DFT_TEX_COLORBIT( 5) )
#define DFT_TEX_FMT_A0I2    ( DFT_TEX_HAS_PALETTE | DFT_TEX_PIXELBIT( 2) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 2) )
#define DFT_TEX_FMT_A0I4    ( DFT_TEX_HAS_PALETTE | DFT_TEX_PIXELBIT( 4) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 4) )
#define DFT_TEX_FMT_A0I8    ( DFT_TEX_HAS_PALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 0) | DFT_TEX_COLORBIT( 8) )
#define DFT_TEX_FMT_A5I3    ( DFT_TEX_HAS_PALETTE | DFT_TEX_PIXELBIT( 8) | DFT_TEX_ALPHABIT( 5) | DFT_TEX_COLORBIT( 3) )
#define DFT_TEX_FMT_A1I15   (                       DFT_TEX_PIXELBIT(16) | DFT_TEX_ALPHABIT( 1) | DFT_TEX_COLORBIT(15) )

typedef struct {
    char *name;
    u32 fmt;
} sTFmt;
extern sTFmt DFT_TEX_FMTS[];

typedef struct sDFT_TEXTURE sDFT_TEXTURE;
struct sDFT_TEXTURE {
    u32 fmt;    // format
    u16 w, h;   // width and height (in pixels ? TODO)
    u8 data[];  // texture data
};

typedef struct sDFT_PALETTE sDFT_PALETTE;
struct sDFT_PALETTE {
    u32 num;
    u16 data[];  // palette data
};

// not used in dsf files
typedef struct sDFT_RANGE sDFT_RANGE;
struct sDFT_RANGE {
    u32 map_offset;
    u32 first_car;
    u32 last_car;
} PACKED;

typedef struct sDFT_RANGES sDFT_RANGES;
struct sDFT_RANGES {
    u32 num;
    sDFT_RANGE ranges[];
};

typedef struct sDFT_GPROPS sDFT_GPROPS;
struct sDFT_GPROPS {
    u16 u:12;
    u16 v:12;
    u8 adx;
    u8 w, h;
    s8 dx, dy;
} PACKED;

// not used in dsf files
typedef struct sDFT_GLYPH sDFT_GLYPH;
struct sDFT_GLYPH {
    sDFT_GPROPS props;
    u8 *bitmap;
    unsigned ccode;
};

typedef struct sDFT_MAP sDFT_MAP;
struct sDFT_MAP {
    u32 num;
    sDFT_GPROPS glyphs[];
};

unsigned    dft_get_tex_fmt (char *str);
int         dft_check       (void *data, unsigned datasize);

#endif

