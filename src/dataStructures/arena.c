#include "../include/dataStructures/arena.h"

size_t arena_getCapacity(const Arena *arena) {
    return arena->capacity;
}

size_t arena_usedBytes(const Arena *arena) {
    return arena->offset;
}

size_t arena_remainingBytes(const Arena *arena) {
    return arena->capacity - arena->offset;
}

int arena_new(void *buffptr, size_t buffSize, Arena **outptr) {
    if (!buffptr) return 0;
    if (!outptr) return 0;
    
    if (sizeof(Arena) > buffSize) return 0;

    Arena *arena = (Arena*) buffptr;
    arena->offset = 0;
    arena->memory = ((uint8_t*)buffptr) + sizeof(Arena);
    arena->capacity = buffSize - sizeof(Arena);

    *outptr = arena;
    return 1;
}

size_t mem_align(size_t ptrOffset, size_t alignment) {
    size_t modulo = ptrOffset & (alignment - 1);

    if (modulo != 0) ptrOffset += alignment - modulo;

    return ptrOffset;
}

void *arena_alloc(Arena *arena, size_t size, size_t align) {
    if (!arena) return NULL;
    if (!align) return NULL;
    if ((align & (align - 1)) != 0) return NULL;

    size_t alignedOffset = mem_align(arena->offset, align);
    if (size > arena->capacity - alignedOffset) return NULL;

    void *ptr = arena->memory + alignedOffset;
    arena->offset = size + alignedOffset;
    return ptr;
}

void arena_reset(Arena *arena) {
    arena->offset = 0;
}



