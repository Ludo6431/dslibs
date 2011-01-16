#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "FT2dft.h"

#include "tools.h"

#include "FTread.h"

void print_glyph(FT_GlyphSlot glyph, FT_Bitmap *bmp) {
    unsigned w = bmp->width;
    unsigned h = bmp->rows;
    unsigned char *buffer = bmp->buffer;

    // print character metrics
    printf(" (\t%dx%d@(%d,%d)\tx+%.1f,y+%.1f\t)\n",
        w, h,                                                       // bitmap size
        glyph->bitmap_left, glyph->bitmap_top,                      // coordinates of the bitmap from the pen
        (float)glyph->advance.x/64.0, (float)glyph->advance.y/64.0  // x and y advance of the pen
    );

    // print character
    int i, j;
    for(j=0; j<h; j++) {
        for(i=0; i<w; i++) {
            unsigned char c = buffer[i + j * w];
            if(c>170) printf("#");
            else if(c>85) printf("*");
            else if(c) printf("+");
            else printf(" ");
        }
        printf("\n");
    }

    // print bitmap data
    for(i=0; i<w*h; i++) printf("%03d|", buffer[i]);
    printf("\n");
}

void FTread(char *ifname, unsigned ofntsize, sSelection *selects, unsigned num_selects, sList *glyphs, sList *ranges, unsigned *_bmpsurface) {
    FT_Library  library;
    FT_Error    error;
    FT_Face     face;
    FT_ULong    charcode;
    FT_UInt     glyphindex;
    FT_Bitmap   bmp;
    unsigned bmpsurface = 0;

    assert(ifname);
    assert(glyphs);

    // init library
    error = FT_Init_FreeType(&library);
    if(error) mexit(1, "ERR: FT_Init_FreeType(&library) failed !");

    // load font
    error = FT_New_Face(library, ifname, 0, &face);
    if(error == FT_Err_Unknown_File_Format)
        mexit(1, "ERR: Unsupported file !");
    else if (error) {
        mexit(1, "ERR: FT_New_Face(library, \"%s\", ...) failed !", ifname);
    }

if(verbose) {
printf("%d num_glyphs\n", (int)face->num_glyphs);
printf("%d num_fixed_sizes\n", face->num_fixed_sizes);
if(face->num_fixed_sizes) {
    unsigned i;
    for(i=0; i<face->num_fixed_sizes; i++)
        printf("  %.2f;%u,%u\n", (float)face->available_sizes[i].size / 64.0, (unsigned int)face->available_sizes[i].x_ppem>>6, (unsigned int)face->available_sizes[i].y_ppem>>6);
}
}

    // set requested size
    error = FT_Set_Pixel_Sizes(face, ofntsize, 0);
    if(error) {
        if(face->num_fixed_sizes) {
            fprintf(stderr, "WARN: Can't set font size %d, trying the fixed size (%ux%u)\n", ofntsize, (unsigned int)face->available_sizes[0].x_ppem>>6, (unsigned int)face->available_sizes[0].y_ppem>>6);
            error = FT_Set_Pixel_Sizes(face, face->available_sizes[0].x_ppem>>6, face->available_sizes[0].y_ppem>>6);
            if(error)   // TODO : try the next sizes if any
                mexit(1, "ERR: FT_Set_Pixel_Sizes(face, %u, %u) failed !", face->available_sizes[0].x_ppem>>6, face->available_sizes[0].y_ppem>>6);
        }
        else
            mexit(1, "ERR: FT_Set_Pixel_Sizes(face, %u, ...) failed !", ofntsize);
    }

    // iter over chars creating ranges and glyphs lists
    FT_ULong sorange, prevccode = (FT_ULong)-2;
    for(
        charcode = FT_Get_First_Char(face, &glyphindex);
        glyphindex;
        charcode = FT_Get_Next_Char(face, charcode, &glyphindex)
    ) {
        if(selects && num_selects  && !sel_contains(selects, num_selects, charcode))
            continue;

        if(ranges) {    // detect jumps
            if(charcode != prevccode+1) {   // will always be valid the first execution of the loop
                if(prevccode != (FT_ULong)-2)   // not the first execution of the loop
                    add_range(ranges, sorange, prevccode);

                sorange = charcode; // update start of range charcode
            }
            prevccode = charcode;
        }

        // load glyph
        error = FT_Load_Glyph(face, glyphindex, FT_LOAD_DEFAULT);
        if(error) mexit(1, "ERR: FT_Load_Glyph(face, %X, ...) failed !", glyphindex);

        // render glyph to bitmap
        error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
        if(error) mexit(1, "ERR: FT_Render_Glyph(face->glyph, ...) failed !");

        // convert to 8bpp
        FT_Bitmap_New(&bmp);
        error = FT_Bitmap_Convert(library, &face->glyph->bitmap, &bmp, 1);
        if(error) mexit(1, "ERR: FT_Bitmap_Convert(...) failed !");
        if(bmp.num_grays != 256) {
            unsigned i;
            for(i=0; i<bmp.width*bmp.rows; i++)
                bmp.buffer[i] = bmp.buffer[i] * 255 / (bmp.num_grays - 1);
        }

if(verbose>1) {
printf("  %08X@%08X",
    (unsigned int)charcode,   // unicode of character
    glyphindex  // glyphindex in face
);
print_glyph(face->glyph, &bmp);
}

        // compute total surface of caracters
        bmpsurface += bmp.width*bmp.rows;

        // add glyph to list
        sDFT_GLYPH *glyph = NEWZ(sDFT_GLYPH);
        assert(glyph);
        glyph->ccode = charcode;
        glyph->props.w = bmp.width;
        glyph->props.h = bmp.rows;
        glyph->props.dx = face->glyph->bitmap_left;
        glyph->props.dy = face->glyph->bitmap_top;
        glyph->props.adx = face->glyph->advance.x >> 6;
        if(glyph->props.w * glyph->props.h) {
            glyph->bitmap = (u8 *)malloc(glyph->props.w * glyph->props.h);
            assert(glyph->bitmap);
            memcpy(glyph->bitmap, bmp.buffer, glyph->props.w * glyph->props.h);
        }
        L_add_tail(glyphs, glyph);

        FT_Bitmap_Done(library, &bmp);
    }

    if(ranges)
        add_range(ranges, sorange, prevccode); // don't forget the last range

    // destroy face
    FT_Done_Face(face);

    // destroy library
    FT_Done_FreeType(library);

    if(_bmpsurface)
        *_bmpsurface = bmpsurface;
}

