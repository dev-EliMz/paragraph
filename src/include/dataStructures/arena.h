#ifndef DSA_ARENA_H
#define DSA_ARENA_H

typedef struct arena Arena;

#include <stdlib.h>
#include <stdint.h>

int arena_new(void* buffptr, size_t buffSize, Arena **outptr);

size_t arena_getCapacity(const Arena *arena);
size_t arena_usedBytes(const Arena *arena);
size_t arena_remainingBytes(const Arena *arena);

void arena_reset(Arena *arena);

/*
 * devEli: align tem que ser potencia de 2, 
 * geralmente a engine vai utilizar alocadores encapsulados mas se voce for utilizar o
 * alocador de arena,
 * passe o alinhamento manualmente ou o size_t do elemento/size_t do elemento mais 
 * estrito da struct a ser alocada
 * O ideal seria utilizar _Alignof(type) (C11)
 * */

void *arena_alloc(Arena *arena, size_t size, size_t align);

struct arena {
    uint8_t *memory;
    size_t capacity;
    size_t offset;
};

#endif
