#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/adata.h"

adata_t ad_new(void *data, unsigned datasize) {
    assert(data);

    adata_t ad_new;

    if(datasize <= sizeof(adata_t))
        memcpy((void *)&ad_new, data, datasize);
    else {
        void *out = malloc(datasize);
        if(!out)
            return 0;
        memcpy(out, data, datasize);
        ad_new = (adata_t)out;
    }

    return ad_new;
}

void *ad_get(adata_t ad, unsigned datasize) {
    static adata_t data;

    if(datasize <= sizeof(adata_t)) {
        data = ad;
        return (void *)&data;
    }
    else
        return (void *)ad;
}

inline void ad_del(adata_t ad, unsigned datasize) {
    if(ad && datasize > sizeof(adata_t))
        free((void *)ad);
}

