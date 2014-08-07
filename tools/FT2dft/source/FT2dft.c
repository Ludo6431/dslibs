#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <getopt.h>
#include <math.h>

#include "tools.h"
#include "selections.h"
#include "limitline.h"
#include "FTread.h"
#include "dump.h"

#include "FT2dft.h"

int verbose = 0;
char *dofname = NULL;     // dump output filename

int main(int argc, char *argv[]) {
    char *      ifname = NULL;      // input filename
    char *      ofname = NULL;      // output filename
    unsigned    ofntsize = 10;      // output font size
    char *      ofmtstr = "A5I3";   // format string
    unsigned    ofmtdesc = DFT_TEX_FMT_A5I3;

    sSelection selects[MAX_SELECTS];
    int num_selects = 0;

    sList ranges, glyphs;
    RESET(&ranges, sList);
    RESET(&glyphs, sList);

    unsigned    bmpsurface;     // total surface of the caracters
    float       bmpavesize;     // average size of the texture (width and height)
    unsigned    bmpfixedwidth;  // chosen width based on the average size

    // parse arguments
    static struct option long_options[] = {             // argument following (if any)
        {"verbose",             no_argument,       0, 'v'}, //
        {"input-file",          required_argument, 0, 'i'}, // filename
        {"output-file",         required_argument, 0, 'o'}, // filename
        {"dump-file-prefix",    required_argument, 0, 'd'}, // filename prefix
        {"texture-format",      required_argument, 0, 'f'}, // texture format
        {"palette-first-trans", no_argument,       0, 't'}, //
        {"font-size",           required_argument, 0, 's'}, // integer (in pixels)
        {"select",              required_argument, 0, 'l'}, // couple of hex integers 0x#:0x# (one of the two maybe omitted)
        {0, 0, 0, 0}
    };
    // TODO : color selection of BG and FG of each face (remove palette-first-trans and replace it with FG=trans)
    // TODO : monochrome ?
    // TODO : usage
    char options_desc[] =
        "vi:o:d:f:ts:l:";
    int option_index;
    int c;
    while(1) {
        option_index = 0;

        c = getopt_long(argc, argv, options_desc, long_options, &option_index);
        if(c == -1) break;

        switch (c) {
        case 0:
            /* If this option set a flag, do nothing else now. */
            if (long_options[option_index].flag != 0)
                break;
            printf ("option %s", long_options[option_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;
        case 'v':
            verbose++;
            break;
        case 'i':   // input file
            ifname = optarg;
            break;
        case 'o':   // output file
            ofname = optarg;
            break;
        case 'd':   // dump file
            dofname = optarg;
            break;
        case 'f':   // texture format
            ofmtstr = optarg;

            ofmtdesc &= ~DFT_TEX_FMTMASK;

            unsigned fmt = dft_get_tex_fmt(ofmtstr);
            if(!fmt)
                mexit(1, "ERR: Invalid texture format \"%s\"", ofmtstr);

            ofmtdesc |= fmt;

            break;
        case 't':
            ofmtdesc |= DFT_TEX_PAL1TRANS;
            break;
        case 's':   // font size
            ofntsize = atoi(optarg);
            break;
        case 'l':   // selection
            if(num_selects >= MAX_SELECTS) mexit(1, "ERR: Too many selections (the max is %d)", MAX_SELECTS);

            if(sel_add(selects, &num_selects, optarg))
                mexit(1, "ERR: Invalid selection argument \"%s\"", optarg);

            break;
        case '?':   // parse error
        default:
            abort();
        }
    }

    // check mandatory arguments and their validity
    // check input filename
    if(!ifname)
        mexit(1, "ERR: No input filename");
    // check ouput filename
    if(!ofname)
        mexit(1, "ERR: No output filename");

    // print input arguments and default values
    printf("Input FreeType font     : %s\n", ifname);
    printf("Output dft font         : %s\n", ofname);
    printf("Output texture format   : %s (%04x)\n", ofmtstr, ofmtdesc);
    printf("Output font size        : %d\n", ofntsize);
    if(num_selects) {
        printf("Convert only this subset of the font :\n");
        for(c=0; c<num_selects; c++)
            printf(selects[c].end?"  0x%08X:0x%08X\n":"  0x%08X:0x__END___\n",selects[c].start, selects[c].end);
    }
    else
        printf("Convert the entire font\n");

    // read the font and fill glyphs, ranges and bmpsurface
    FTread(ifname, ofntsize, selects, num_selects, &glyphs, &ranges, &bmpsurface);

    bmpavesize = sqrt((float)bmpsurface);
    bmpfixedwidth = (int)pow(2, ceil(log(bmpavesize)/log(2)));  // the next 2^k

if(verbose) {
    printf("%d range(s)\n", ranges.count);
    printf("%d glyphs(s)\n", glyphs.count);
    printf("%d surface (px²)\n", bmpsurface);
    printf("%.1fx%.1f average size (px)\n", bmpavesize, bmpavesize);
    printf("%d fixed width (px)\n", bmpfixedwidth);
}

    // dump size data
    dump_glyphs_size(dofname, &glyphs);

// we now have the data in ranges and glyphs, we can create the dft file

    unsigned w = bmpfixedwidth, h;

    // optimise the position of the caracters on the texture (with a fixed width)
    genplacement(&glyphs, w, &h);

if(verbose)
printf("texsize=%dx%d\n", w, h);
if(verbose)
printf("efficiency = %.1f%%\n", 100.0*(float)bmpsurface/(float)(w*h));

    // dump position data
    dump_glyphs_positions(dofname, &glyphs);

    // write to dft file
    sDFT_RANGES *dftranges;
    sDFT_MAP *dftmap;
    sDFT_TEXTURE *dfttexture;
    sDFT_PALETTE *dftpalette;

    dft_new_ranges(&ranges, &dftranges);
    dft_new_map(&glyphs, &dftmap);
    dft_new_texture(&glyphs, w, h, ofmtdesc, &dfttexture, &dftpalette);

    dft_write(ofname, dftranges, dftmap, dfttexture, dftpalette);

    dft_cleanup(dftranges, dftmap, dfttexture, dftpalette);

    // free data
    void delglyph(sDFT_GLYPH *glyph) {
        if(glyph->bitmap) free(glyph->bitmap);
        free(glyph);
    }
    L_dropall(&glyphs, (datahandler)delglyph);
    L_dropall(&ranges, free);

    if(verbose)
        malloc_stats();
}

