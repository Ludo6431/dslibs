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
#define _GET_BLOCK(sizelist, blockslist, i) ((void *)&(  ((char *)(blockslist))[sizeof(struct blist) + (i)*((sizelist)->block_size)]  ))

struct sllist {
    struct sllist *next;

    size_t block_size;

    size_t nnbblocks;   // the next number of blocks
    struct blist *last_list;
};

static struct sllist *list = NULL;

void *slice_alloc(size_t block_size) {
    struct sllist *tmp = list, *prev = NULL;

    block_size = MEMALIGN4(block_size);

    if(block_size>SIZE_THRESHOLD)
        return malloc(block_size);
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

        tmp->nnbblocks = FIRST_NBBLOCKS; // initial number of blocks
        tmp->block_size = block_size;
    }

    // ok, we can now find a free bloc in tmp
    unsigned int i;
    struct blist *btmp = tmp->last_list;
    void *ret = NULL;

    while(btmp) {
        for(i=btmp->first_free; i<btmp->nbblocks; i++)
            if(! (btmp->free_mask & BIT(i)) ) {    // there is a free block
                if(!ret) {
                    btmp->free_mask |= BIT(i);
                    ret = _GET_BLOCK(tmp, btmp, i);
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
    btmp = malloc(sizeof(struct blist) + tmp->nnbblocks*tmp->block_size);
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
    return _GET_BLOCK(tmp, btmp, 0);
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
    struct sllist *tmp, *prev = NULL;
    struct blist *btmp, *bprev = NULL;

    block_size = MEMALIGN4(block_size);

    if(block_size>SIZE_THRESHOLD)
        free(mem_block);
    // search the size
    tmp = list;
    while(tmp) {    // search if it already exists
        if(tmp->block_size == block_size)
            break;

        prev = tmp;
        tmp = tmp->next;
    }

    if(!tmp)
        return; // unknown block size

    // search the block list
    btmp = tmp->last_list;
    while(btmp) {
        if(_GET_BLOCK(tmp, btmp, 0) <= mem_block && mem_block <= _GET_BLOCK(tmp, btmp, btmp->nbblocks-1))
            break;

        bprev = btmp;
        btmp = btmp->next;
    }

    if(!btmp)   // unknown block pointer
        return;

    // mark the block as free
    unsigned int bit = (unsigned int)(mem_block - _GET_BLOCK(tmp, btmp, 0))/tmp->block_size;

    btmp->free_mask &= ~BIT(bit);
    btmp->first_free = MIN(btmp->first_free, bit);

    if(!btmp->free_mask) {  // the block list is empty, we remove it
        if(bprev)
            bprev->next = btmp->next;
        else
            tmp->last_list = btmp->next;

        free(btmp);

        if(!tmp->last_list) {   // it was the last block list of this size
            if(prev)
                prev->next = tmp->next;
            else
                list = tmp->next;

            free(tmp);
        }
    }
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

    fprintf(fd, "List(s) of %d(%d)bytes blocks :\n", block_size, _block_size);

    for(btmp = tmp->last_list, i = 0; btmp; btmp = btmp->next, i++) {
        fprintf(fd, "list #%d:\n", i+1);
        fprintf(fd, "\t%d blocks\n", btmp->nbblocks);
        fprintf(fd, "\tfree blocks: %08x (%u)\n", btmp->free_mask, btmp->first_free);
    }
}

