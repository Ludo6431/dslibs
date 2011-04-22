#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/adata.h"

unsigned ad_new(void *data, unsigned datasize) {
    assert(data);

    unsigned ad_new;

    if(datasize <= sizeof(unsigned))
        memcpy((void *)&ad_new, data, datasize);
    else {
        void *out = malloc(datasize);
        if(!out)
            return 0;
        memcpy(out, data, datasize);
        ad_new = (unsigned)out;
    }

    return ad_new;
}

void *ad_get(unsigned ad, unsigned datasize) {
    static unsigned data;

    if(datasize <= sizeof(unsigned)) {
        data = ad;
        return (void *)&data;
    }
    else
        return (void *)ad;
}

inline void ad_del(unsigned ad, unsigned datasize) {
    if(ad && datasize > sizeof(unsigned))
        free((void *)ad);
}

