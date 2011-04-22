#ifndef _ADATA_H
#define _ADATA_H

unsigned        ad_new  (void *data, unsigned datasize);
void *          ad_get  (unsigned ad, unsigned datasize);
inline void     ad_del  (unsigned ad, unsigned datasize);

#endif

