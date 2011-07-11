#ifndef _SLICE_H
#define _SLICE_H

void *          slice_alloc     (size_t block_size);
inline void *   slice_alloc0    (size_t block_size);
inline void *   slice_copy      (size_t block_size, void *mem_block);
void            slice_free      (size_t block_size, void *mem_block);

// TODO: slice_free_chain

#define         slice_new(t)        ((t *)slice_alloc(sizeof(t)))
#define         slice_dup(t, m)     ((t *)slice_copy(sizeof(t), m))
#define         slice_delete(t, m)  slice_free(sizeof(t), m)

// ------------ DEBUG ------------

void            slice_dump      (size_t block_size);

#endif

