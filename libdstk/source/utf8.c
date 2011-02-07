#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/utf8.h"

int utf8_strlen(char *s) {
    int len=0, csz;

    while(*s && (csz = utf8_cssize(s))) {
        s+=csz;
        len++;
    }

    return len;
}

inline int utf8_strsize(char *s) {
    return strlen(s)+1;
}

int utf8_cssize(char *cs) {
    assert(cs);

    if(cs[0] < 0x80) {
        /* 0xxxxxxx */
        return 1;
    }

    if((cs[1] & 0xc0) != 0x80) return 0;
    if((cs[0] & 0xe0) == 0xc0) {
        /* 110xxxxx 10xxxxxx */
        return 2;
    }

    if((cs[2] & 0xc0) != 0x80) return 0;
    if((cs[0] & 0xf0) == 0xe0) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        return 3;
    }

    if((cs[3] & 0xc0) != 0x80) return 0;
    if((cs[0] & 0xf8) == 0xf0) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        return 4;
    }

    if((cs[4] & 0xc0) != 0x80) return 0;
    if((cs[0] & 0xfc) == 0xf8) {
        /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        return 5;
    }

    if((cs[5] & 0xc0) != 0x80) return 0;
    if((cs[0] & 0xfe) == 0xfc) {
        /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        return 6;
    }

    return 0;
}

int utf8_wc2cs(wchar32 wc, char *cs, unsigned cslen) {  // from libiconv @ utf8.h
    assert(cs);

    int count;
    if(wc < 0x80)
        count = 1;
    else if(wc < 0x800)
        count = 2;
    else if(wc < 0x10000)
        count = 3;
    else if(wc < 0x200000)
        count = 4;
    else if(wc < 0x4000000)
        count = 5;
    else if(wc <= 0x7fffffff)
        count = 6;
    else
        return -1;
    if(count > cslen)
        return -1;

    switch(count) {
    case 6: cs[5] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x4000000;
    case 5: cs[4] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x200000;
    case 4: cs[3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
    case 3: cs[2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
    case 2: cs[1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
    case 1: cs[0] = wc;
    }

    return count;
}

wchar32 utf8_cs2wc(char *cs, int *cssize) {
    assert(cs);

    if(cs[0] < 0x80) {
        /* 0xxxxxxx */
        if(cssize) *cssize = 1;
        return cs[0];
    }

    if((cs[1] & 0xc0) != 0x80) return -1;
    if((cs[0] & 0xe0) == 0xc0) {
        /* 110xxxxx 10xxxxxx */
        if(cssize) *cssize = 2;
        return
            ((cs[0] & 0x1f) << 6) |
             (cs[1] & 0x3f);
    }

    if((cs[2] & 0xc0) != 0x80) return -1;
    if((cs[0] & 0xf0) == 0xe0) {
        /* 1110xxxx 10xxxxxx 10xxxxxx */
        if(cssize) *cssize = 3;
        return
            ((cs[0] & 0x0f) << 12) |
            ((cs[1] & 0x3f) <<  6) |
             (cs[2] & 0x3f);
    }

    if((cs[3] & 0xc0) != 0x80) return -1;
    if((cs[0] & 0xf8) == 0xf0) {
        /* 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if(cssize) *cssize = 4;
        return
            ((cs[0] & 0x07) << 18) |
            ((cs[1] & 0x3f) << 12) |
            ((cs[2] & 0x3f) <<  6) |
             (cs[3] & 0x3f);
    }

    if((cs[4] & 0xc0) != 0x80) return -1;
    if((cs[0] & 0xfc) == 0xf8) {
        /* 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if(cssize) *cssize = 5;
        return
            ((cs[0] & 0x03) << 24) |
            ((cs[1] & 0x3f) << 18) |
            ((cs[2] & 0x3f) << 12) |
            ((cs[3] & 0x3f) <<  6) |
             (cs[4] & 0x3f);
    }

    if((cs[5] & 0xc0) != 0x80) return -1;
    if((cs[0] & 0xfe) == 0xfc) {
        /* 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        if(cssize) *cssize = 6;
        return
            ((cs[0] & 0x01) << 30) |
            ((cs[1] & 0x3f) << 24) |
            ((cs[2] & 0x3f) << 18) |
            ((cs[3] & 0x3f) << 12) |
            ((cs[4] & 0x3f) <<  6) |
             (cs[5] & 0x3f);
    }

    return -1;
}

wchar32 utf8_iter(UTF8_ITER *iter, char *init) {
    if(init)
        *iter = init;

    int size;

    wchar32 ret = utf8_cs2wc((char *)*iter, &size);

    if(ret>=0)
        (*iter) += size;

    return ret;
}

