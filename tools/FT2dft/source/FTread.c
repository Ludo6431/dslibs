#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "FT2dft.h"

#include "tools.h"

#include "FTread.h"

void print_glyph(FT_GlyphSlot glyph) {
    unsigned w = glyph->bitmap.width;
    unsigned h = glyph->bitmap.rows;
    unsigned char *buffer = glyph->bitmap.buffer;

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

printf("%d num_glyphs\n", (int)face->num_glyphs);
printf("%d num_fixed_sizes\n", face->num_fixed_sizes);

    // set requested size
    error = FT_Set_Pixel_Sizes(face, ofntsize, 0);
    if(error) mexit(1, "ERR: FT_Set_Pixel_Sizes(face, %u, ...) failed !", ofntsize);

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

if(verbose>1) {
printf("  %08X@%08X",
    (unsigned int)charcode,   // unicode of character
    glyphindex  // glyphindex in face
);
print_glyph(face->glyph);
}

        bmpsurface += face->glyph->bitmap.width*face->glyph->bitmap.rows;

        add_glyph(glyphs, face->glyph, charcode);
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

