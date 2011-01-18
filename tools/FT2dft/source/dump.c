#include <stdlib.h>
#include <stdio.h>
#include "FT2dft.h"

#include "tools.h"
#include "bmpsave.h"

#include "dump.h"

void dump_glyphs_size(char *dofname, sList *glyphs) {
    if(!dofname) return;

    char *fn = add_suffix(dofname, ".glyphs_sizes.txt");

    FILE *dofd = fopen(fn, "wb");

    if(!dofd)
        fprintf(stderr, "WARN: Can't dump glyphs to file \"%s\"\n", fn);
    else {
        fprintf(stderr, "INFO: dumping glyphs to file \"%s\"\n", fn);

        fprintf(dofd, "%d\n# charcode width height\n", glyphs->count);
        sLEl *curr = glyphs->head;
        while(curr) {
            fprintf(dofd, "%08X %d %d\n", SGLYPH(curr)->ccode, SGLYPH(curr)->props.w, SGLYPH(curr)->props.h);

            curr = curr->next;
        }

        fclose(dofd);
    }
}

void dump_glyphs_positions(char *dofname, sList *glyphs) {
    if(!dofname) return;

    char *fn = add_suffix(dofname, ".glyphs_positions.txt");

    FILE *dofd = fopen(fn, "wb");

    if(!dofd)
        fprintf(stderr, "WARN: Can't dump glyphs positions to file \"%s\"\n", fn);
    else {
        fprintf(stderr, "INFO: dumping glyphs positions to file \"%s\"\n", fn);

        fprintf(dofd, "%d\n# charcode width height u v\n", glyphs->count);
        sLEl *curr = glyphs->head;
        while(curr) {
            fprintf(dofd, "%08X %d %d %d %d\n", SGLYPH(curr)->ccode, SGLYPH(curr)->props.w, SGLYPH(curr)->props.h,SGLYPH(curr)->props.u, SGLYPH(curr)->props.v);

            curr = curr->next;
        }

        fclose(dofd);
    }
}

void dump_texture(char *dofname, char *tex, unsigned w, unsigned h) {
    if(!dofname) return;

    char *fn = add_suffix(dofname, ".tex.bmp");
    fprintf(stderr, "INFO: dumping texture to file \"%s\" (%ux%u)\n", fn, w, h);
    bmpsave(fn, tex, w, h);
}

