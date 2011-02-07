#ifndef _UTF8_H
#define _UTF8_H

typedef signed int wchar32; // 31 bits of data, 1 bit of error

typedef char * UTF8_ITER;

int         utf8_strlen     (char *s);  // number of characters in an utf8 string
inline int  utf8_strsize    (char *s);  // size in bytes of an utf8 string
int         utf8_cssize     (char *cs); // size in bytes of an utf8 character
int         utf8_wc2cs      (wchar32 wc, char *cs, unsigned cslen); // wide char to utf8 character
wchar32     utf8_cs2wc      (char *cs, int *cssize); // utf8 character to wide char (and optionnaly get the size of the utf8 character)
wchar32     utf8_iter       (UTF8_ITER *iter, char *init);  // iter over wide chars in utf8 string

#endif

