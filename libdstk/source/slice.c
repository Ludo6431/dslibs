#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/common.h"

#include "dstk/slice.h"

#define MEMALIGN4(n) ((unsigned int)((n) + 3) & ~((unsigned int)3))
#ifndef BIT
#   define BIT(b) (1<<(b))
#endif

struct blist {
    struct blist *next;

    size_t nbblocks;
    unsigned int free_mask;
};  // the data is allocated right after the blist in memory, hence the _GET_BLOCK define :
#define _GET_BLOCK(sizelist, blockslist, i) ((void *)&(  ((char *)(blockslist))[sizeof(struct blist) + (i)*((sizelist)->block_size)]  ))

struct sllist {
    struct sllist *next;

    size_t block_size;

    size_t nblists;
    struct blist *last_list;
};

static struct sllist *list = NULL;

void *slice_alloc(size_t block_size) {
    struct sllist *tmp = list, *prev = NULL;

    block_size = MEMALIGN4(block_size);

    while(tmp) {    // search if it already exists
        if(tmp->block_size == block_size)
            break;

        prev = tmp;
        tmp = tmp->next;
    }

    if(tmp) {
        if(prev) {  // if i'm not already the first blockslist in the list
            prev->next = tmp->next;
            tmp->next = list;
            list = tmp;
        }
    }
    else {  // not existing, prepend new blockslist
        tmp = calloc(1, sizeof(struct sllist));
        if(!tmp)
            return NULL;

        tmp->next = list;
        list = tmp;

        tmp->block_size = block_size;
    }

    // ok, we can now find a free bloc in tmp
    unsigned int i;
    struct blist *btmp = tmp->last_list;

    while(btmp) {
        for(i=0; i<btmp->nbblocks; i++)
            if(! (btmp->free_mask & BIT(i)) ) {    // there is a free block
                btmp->free_mask |= BIT(i);
                return _GET_BLOCK(tmp, btmp, i);
            }

        btmp = btmp->next;
    }

    // there isn't a free block => we add a list with the double of blocks
    tmp->nblists++;

    size_t nbblocks = MIN(4*(1<<(tmp->nblists-1)), 32);

    btmp = malloc(sizeof(struct blist) + nbblocks*tmp->block_size);
    if(!btmp)
        return NULL;
    bzero(btmp, sizeof(struct blist));

    // (prepend the new blocks list)
    btmp->next = tmp->last_list;
    tmp->last_list = btmp;
    btmp->nbblocks = nbblocks;

    // use the last block we just allocated
    btmp->free_mask |= 1<<(nbblocks-1);
    return _GET_BLOCK(tmp, btmp, nbblocks-1);
}

inline void *slice_alloc0(size_t block_size) {
    void *ret = slice_alloc(block_size);
    memset(ret, 0, block_size);
    return ret;
}

inline void *slice_copy(size_t block_size, void *mem_block) {
    void *ret = slice_alloc(block_size);
    memcpy(ret, mem_block, block_size);
    return ret;
}

void slice_free(size_t block_size, void *mem_block) {
    struct sllist *tmp = list;
    struct blist *btmp;

    block_size = MEMALIGN4(block_size);

    while(tmp) {    // search if it already exists
        if(tmp->block_size == block_size)
            break;

        tmp = tmp->next;
    }

    if(!tmp)
        return; // unknown block size

    btmp = tmp->last_list;
    while(btmp) {
        if(_GET_BLOCK(tmp, btmp, 0) <= mem_block && mem_block <= _GET_BLOCK(tmp, btmp, btmp->nbblocks-1)) {
            btmp->free_mask &= ~BIT((unsigned int)(mem_block - _GET_BLOCK(tmp, btmp, 0))/tmp->block_size);
            return;
        }

        btmp = btmp->next;
    }

    // unknown block pointer
}

// --------------- DEBUG --------------

void slice_dump(size_t _block_size, FILE *fd) {
    struct sllist *tmp = list;
    struct blist *btmp;
    size_t block_size;
    int i;

    block_size = MEMALIGN4(_block_size);

    while(tmp) {    // search if it already exists
        if(tmp->block_size == block_size)
            break;

        tmp = tmp->next;
    }

    if(!tmp) {
        fprintf(fd, "Unknown block size\n");
        return; // unknown block size
    }

    fprintf(fd, "There are %d list(s) of %d(%d)bytes blocks\n", tmp->nblists, block_size, _block_size);

    for(btmp = tmp->last_list, i = 0; btmp; btmp = btmp->next, i++) {
        fprintf(fd, "list #%d:\n", i+1);
        fprintf(fd, "\t%d blocks\n", btmp->nbblocks);
        fprintf(fd, "\tfree blocks: %08x\n", btmp->free_mask);
    }
}

