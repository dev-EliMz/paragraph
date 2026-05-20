#ifndef DSA_ALLOCATORS_H
#define DSA_ALLOCATORS_H

#include "arena.h"

typedef struct {
    void *context;

    void *(*alloc)(
            void *context,
            size_t size,
            size_t alignment
            );

    void (*free)(
            void *context,
            void *ptr
            );
} DsaAllocator;

void *heap_allocator(void *context, size_t size, size_t alignment) {
    return malloc(size);
}

void *arena_allocator(void *context, size_t size, size_t alignment) {
    Arena *arena = (Arena*) context;
    return arena_alloc(arena, size, alignment);
}

void heap_free(void *context, void *ptr) {
    if (!ptr) return;
    free(ptr);
}

void *dsa_memmove();

#endif
