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
    _FMT(A1I15),
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
#ifdef ARM9
    if(swiCRC16(0xFFFF, data, eoffset) != eof->crc)
        return 1;
#else
    printf("Current CRC16 = %04X\n", eof->crc); // TODO
#endif

    return 0;
}

