#ifndef _COMMON_H
#define _COMMON_H

void mexit(int ret, char *format, ...);

#define NEW(type) (type *)malloc(sizeof(type))
#define RESET(d, type) bzero((type *)(d), sizeof(type))
#define NEWZ(type) (type *)calloc(1, sizeof(type))

#endif

