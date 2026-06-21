#ifndef DSA_DYNAMICARRAY_H 
#define DSA_DYNAMICARRAY_H 

typedef struct dynarray DsaDynArray;

#include "allocators.h"
#include <string.h>



/*
 * O alocador deve permanecer valido durante a vida do array e de
 * outras estruturas de dados que utilizam o alocador
 */

size_t dsa_array_getElementCount(DsaDynArray *array);
size_t dsa_array_getArrayCapacity(DsaDynArray *array);

int dsa_array_new(
        DsaAllocator *allocator,
        size_t elementSize, size_t capacity, size_t align, float expansion,
        DsaDynArray **outputPtr);

int dsa_array_expand(DsaDynArray *array);
void dsa_array_destroy(DsaDynArray **arrayPtr);
void *dsa_array_reset(DsaDynArray *array);
 /* int dsa_array_copy(DsaDynArray *dest, DsaDynArray *src); */
int dsa_array_reserve(DsaDynArray *array, size_t minCapacity);
int dsa_array_resize_internal(DsaDynArray *array, size_t newCapacity);
/*
 * use push e get para acessar os enderecos de memoria
 * do array. Caso o array cresca, os enderecos ficam invalidos,
 * um endereco retornado antes de uma insercao no array tem chance de
 * estar invalido pois o array pode crescer na insercao
 *
 */

void *dsa_array_push(DsaDynArray *array);

int dsa_array_safe_get(DsaDynArray *array, size_t index, void **outPtr);
void *dsa_array_get(DsaDynArray *array, size_t index);


/* 
 * Dsa pop remove o ultimo elemento do array, o ponteiro retornado continua 
 * valido ate a proxima operacao de push, diferentemente da funcao peek, que
 * apenas retorna o ultimo elemento, sem invalidar o ponteiro
 */

void *dsa_array_pop(DsaDynArray *array);
void *dsa_array_peek(DsaDynArray *array);

#endif
