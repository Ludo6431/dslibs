#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "dstk/common.h"

#include "dstk/slice.h"

#define MEMALIGN4(n) ((unsigned int)((n) + 3) & ~((unsigned int)3))
#ifndef BIT
#   define BIT(b) (1<<(b))
#endif

#define FIRST_NBBLOCKS 4
#define SIZE_THRESHOLD 64

struct blist {
    struct blist *next;

    unsigned short nbblocks;
    unsigned short first_free;
    unsigned int free_mask;
};  // the data is allocated right after the blocklist in memory, hence the _GET_BLOCK define :
#define _GET_BLOCK(blocksize, blockslist, i) ((void *)&(  ((char *)(blockslist))[sizeof(struct blist) + (i)*(blocksize)]  ))

struct sllist {
    size_t nnbblocks;   // the next number of blocks
    struct blist *last_list;
} list[SIZE_THRESHOLD>>2] = {{0}};

void *slice_alloc(size_t block_size) {
    struct sllist *tmp;

    if(!block_size)
        return NULL;

    block_size = MEMALIGN4(block_size);

    if(block_size>SIZE_THRESHOLD)
        return malloc(block_size);

    // get the right size
    tmp = &list[(block_size>>2)-1];

    if(!tmp->nnbblocks)
        tmp->nnbblocks = FIRST_NBBLOCKS; // initial number of blocks

    // ok, we can now find a free bloc in tmp
    unsigned int i;
    struct blist *btmp = tmp->last_list;
    void *ret = NULL;

    while(btmp) {
        for(i=btmp->first_free; i<btmp->nbblocks; i++)
            if(! (btmp->free_mask & BIT(i)) ) {    // there is a free block
                if(!ret) {
                    btmp->free_mask |= BIT(i);
                    ret = _GET_BLOCK(block_size, btmp, i);
                }
                else {
                    btmp->first_free = i;
                    return ret;
                }
            }
        if(ret)
            return ret;

        btmp = btmp->next;
    }

    // there isn't a free block => we add a list with the double of blocks
    btmp = malloc(sizeof(struct blist) + tmp->nnbblocks*block_size);
    if(!btmp)
        return NULL;
    bzero(btmp, sizeof(struct blist));

    // (prepend the new blocks list)
    btmp->next = tmp->last_list;
    tmp->last_list = btmp;
    btmp->nbblocks = tmp->nnbblocks;

    // increment the next number of blocks
    if(tmp->nnbblocks<32)
        tmp->nnbblocks<<=1;

    // use the first block we just allocated
    btmp->free_mask = BIT(0);   // the first block is used
    btmp->first_free = 1;   // the second block is free
    return _GET_BLOCK(block_size, btmp, 0);
}

inline void *slice_alloc0(size_t block_size) {
    void *ret = slice_alloc(block_size);
    if(ret)
        memset(ret, 0, block_size);
    return ret;
}

inline void *slice_copy(size_t block_size, void *mem_block) {
    void *ret = slice_alloc(block_size);
    if(ret)
        memcpy(ret, mem_block, block_size);
    return ret;
}

void slice_free(size_t block_size, void *mem_block) {
    struct sllist *tmp;
    struct blist *btmp, *bprev = NULL;

    block_size = MEMALIGN4(block_size);

    if(block_size>SIZE_THRESHOLD)
        free(mem_block);

    // get the right size
    tmp = &list[(block_size>>2)-1];

    // search the block list
    btmp = tmp->last_list;
    while(btmp) {
        if(_GET_BLOCK(block_size, btmp, 0) <= mem_block && mem_block <= _GET_BLOCK(block_size, btmp, btmp->nbblocks-1))
            break;

        bprev = btmp;
        btmp = btmp->next;
    }

    if(!btmp)   // unknown block pointer
        return;

    // mark the block as free
    unsigned int bit = (unsigned int)(mem_block - _GET_BLOCK(block_size, btmp, 0))/block_size;

    btmp->free_mask &= ~BIT(bit);
    btmp->first_free = MIN(btmp->first_free, bit);

    if(!btmp->free_mask) {  // the block list is empty, we remove it
        if(bprev)
            bprev->next = btmp->next;
        else
            tmp->last_list = btmp->next;

        free(btmp);
    }
}

// --------------- DEBUG --------------

void slice_dump(size_t _block_size, FILE *fd) {
    struct sllist *tmp;
    struct blist *btmp;
    size_t block_size;
    int i;

    block_size = MEMALIGN4(_block_size);

    if(block_size>SIZE_THRESHOLD) {
        fprintf(fd, "Size too big\n");
        return;
    }

    tmp = &list[(block_size>>2)-1];

    if(!tmp->last_list) {
        fprintf(fd, "Unknown block size\n");
        return;
    }

    fprintf(fd, "List(s) of %d(%d)bytes blocks :\n", block_size, _block_size);

    for(btmp = tmp->last_list, i = 0; btmp; btmp = btmp->next, i++) {
        fprintf(fd, "list #%d:\n", i+1);
        fprintf(fd, "\t%d blocks\n", btmp->nbblocks);
        fprintf(fd, "\tfree blocks: %08x (%u)\n", btmp->free_mask, btmp->first_free);
    }
}

