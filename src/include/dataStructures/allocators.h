#ifndef DSA_ALLOCATORS_H
#define DSA_ALLOCATORS_H

#include "arena.h"
#include <stdlib.h>

typedef struct {
    void *context;

    void *(*alloc)(
            void *context,
            size_t size,
            size_t alignment
            );
    
    void *(*realloc)(
            void *context,
            void *ptr,
            size_t size
            );

    void (*free)(
            void *context,
            void *ptr
            );
} DsaAllocator;

typedef struct {
    size_t elementSize;
    size_t alignment;
    float expFactor;
} DsaDynamicConfig;

void *heap_allocator(void *context, size_t size, size_t alignment) {
    (void)alignment;
    (void)context;
    return malloc(size);
}

void *arena_allocator(void *context, size_t size, size_t alignment) {
    Arena *arena = (Arena*) context;
    return arena_alloc(arena, size, alignment);
}

void heap_free(void *context, void *ptr) {
    (void)context;
    if (!ptr) return;
    free(ptr);
}

void *heap_reallocator(void *context, void *ptr, size_t size) {
    (void)context;
    return realloc(ptr, size);
}

DsaDynamicConfig dsa_dynamic_config_init(
        size_t elementSize,
        size_t alignment,
        float expFactor
        ) {
    DsaDynamicConfig config = { elementSize, alignment, expFactor };
    return config;
}

void *dsa_memmove();

#endif
