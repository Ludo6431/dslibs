#ifndef _ADATA_H
#define _ADATA_H

typedef unsigned adata_t;

adata_t         ad_new  (void *data, unsigned datasize);
void *          ad_get  (adata_t ad, unsigned datasize);
inline void     ad_del  (adata_t ad, unsigned datasize);

#endif

