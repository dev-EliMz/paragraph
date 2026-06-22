#include "core/arena.h"

size_t dsa_arena_getCapacity(const DsaArena *arena) {
    return arena->capacity;
}

size_t dsa_arena_usedBytes(const DsaArena *arena) {
    return arena->offset;
}

size_t dsa_arena_remainingBytes(const DsaArena *arena) {
    return arena->capacity - arena->offset;
}

int dsa_arena_new(void *buffptr, size_t buffSize, DsaArena **outptr) {
    if (!buffptr) return 0;
    if (!outptr) return 0;
    
    if (sizeof(DsaArena) > buffSize) return 0;

    DsaArena *arena = (DsaArena*) buffptr;
    arena->offset = 0;
    arena->memory = ((uint8_t*)buffptr) + sizeof(DsaArena);
    arena->capacity = buffSize - sizeof(DsaArena);

    *outptr = arena;
    return 1;
}

size_t mem_align(size_t ptrOffset, size_t alignment) {
    size_t modulo = ptrOffset & (alignment - 1);

    if (modulo != 0) ptrOffset += alignment - modulo;

    return ptrOffset;
}

void *dsa_arena_alloc(DsaArena *arena, size_t size, size_t align) {
    if (!arena) return NULL;
    if (!align) return NULL;
    if ((align & (align - 1)) != 0) return NULL;

    size_t alignedOffset = mem_align(arena->offset, align);
    if (size > arena->capacity - alignedOffset) return NULL;

    void *ptr = arena->memory + alignedOffset;
    arena->offset = size + alignedOffset;
    return ptr;
}

void dsa_arena_reset(DsaArena *arena) {
    arena->offset = 0;
}

struct dsa_snapshot {
    DsaArena *arena;
    size_t offsetSnapshot;
};

int dsa_arena_snapshot_new(DsaArena *arena, DsaArenaSnapshot **outputPtr) {
    if (!arena) return 0;
    if (!outputPtr) return 0;

    DsaArenaSnapshot *snap = dsa_arena_alloc(
            arena,
            sizeof(DsaArenaSnapshot), 
            _Alignof(DsaArenaSnapshot)
            );
    
    if (!snap) return 0;

    snap->arena = arena;
    snap->offsetSnapshot = dsa_arena_usedBytes(arena);
    *outputPtr = snap;

    return 1;
} 

int dsa_arena_rollback(DsaArenaSnapshot **snapPtr) {
    if (!snapPtr || !(*snapPtr) || !(*snapPtr)->arena) return 0;

    DsaArena *arena = (*snapPtr)->arena;
    arena->offset = (*snapPtr)->offsetSnapshot;
    
    *snapPtr = NULL;
    return 1;
}
