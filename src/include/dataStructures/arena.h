#ifndef DSA_ARENA
#define DSA_ARENA

#include <stdint.h>
typedef struct arena Arena;

#include <stdlib.h>

int arena_new(void* buffptr, size_t buffSize, Arena **outptr);
size_t arena_struct_size();

size_t arena_getSize(Arena *arena);
size_t arena_usedBytes(Arena *arena);

void arena_reset(Arena *arena);
void *arena_alloc(Arena *arena, size_t size);

struct arena {
    uint8_t *memory;
    size_t capacity;
    size_t offset;
};

size_t arena_struct_size() {
    return sizeof(Arena);
}

size_t arena_getSize(Arena *arena) {
    return arena->capacity;
}

size_t arena_usedBytes(Arena *arena) {
    return arena->capacity - arena->offset;
}

int arena_new(void *buffptr, size_t buffSize, Arena **outptr) {
    if (!buffptr) return 0;
    if (!outptr) return 0;
    
    if (arena_struct_size() > buffSize) return 0;

    Arena *arena = (Arena*) buffptr;
    arena->offset = 0;
    arena->memory = ((uint8_t*)buffptr) + arena_struct_size();
    arena->capacity = buffSize - arena_struct_size();

    *outptr = arena;
    return 1;
}

void *arena_alloc(Arena *arena, size_t size) {
    if (!arena) return NULL;
    if (size > arena->capacity - arena->offset) return NULL;

    void *ptr = arena->memory + arena->offset;
    arena->offset += size;
    return ptr;
}

void arena_reset(Arena *arena) {
    arena->offset = 0;
}

    
#endif
