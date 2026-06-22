#ifndef DSA_ARENA_H
#define DSA_ARENA_H

typedef struct arena DsaArena;

#include <stddef.h>
#include <stdint.h>

int dsa_arena_new(void* buffptr, size_t buffSize, DsaArena **outptr);

size_t dsa_arena_getCapacity(const DsaArena *arena);
size_t dsa_arena_usedBytes(const DsaArena *arena);
size_t dsa_arena_remainingBytes(const DsaArena *arena);

void dsa_arena_reset(DsaArena *arena);

/*
 * align tem que ser potencia de 2, 
 * geralmente a engine vai utilizar alocadores encapsulados mas se voce for utilizar o
 * alocador de arena,
 * passe o alinhamento manualmente ou o size_t do elemento/size_t do elemento mais 
 * estrito da struct a ser alocada
 * O ideal seria utilizar _Alignof(type) (C11)
 * */

void *dsa_arena_alloc(DsaArena *arena, size_t size, size_t align);

struct arena {
    uint8_t *memory;
    size_t capacity;
    size_t offset;
};

size_t mem_align(size_t ptrOffset, size_t alignment);

//macro com alignof (C11) TODO: incluir ifdefs para validar a versao C do compilador
#define dsa_arena_alloc_t(arena, T) \
    (T*)dsa_arena_alloc(arena, sizeof(T), _Alignof(T))

// DsaArena Snapshots
typedef struct dsa_snapshot DsaArenaSnapshot;

int dsa_arena_snapshot_new(DsaArena *arena, DsaArenaSnapshot **outputPtr);
int dsa_arena_rollback(DsaArenaSnapshot **snapPtr);

#endif
