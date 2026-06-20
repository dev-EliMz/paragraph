#ifndef DSA_DYNAMICARRAY_H 
#define DSA_DYNAMICARRAY_H 

typedef struct dynarray DsaDynArray;

#include "allocators.h"
#include <string.h>

struct dynarray {
    uint8_t *data;
    size_t count;
    size_t capacity;
    size_t elementSize;
    size_t alignment;
    float expFactor;
    DsaAllocator *allocator;
};

/*
 * O alocador deve permanecer valido durante a vida do array e de
 * outras estruturas de dados que utilizam o alocador
 */

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

int dsa_array_new(
        DsaAllocator *allocator, 
        size_t elementSize, size_t capacity, size_t align, float expansion,
        DsaDynArray **outputPtr
        ) {
    
    if (!outputPtr) return 0;
    if (!elementSize || capacity <= 0 || expansion < 1.0f) return 0;
    if ((align & (align - 1)) != 0) return 0;
    if (!allocator || !(allocator->alloc)) return 0;
    
    DsaDynArray *array = allocator->alloc(
            allocator->context, 
            sizeof(DsaDynArray), 
            _Alignof(DsaDynArray)
            );

    if (!array) return 0;
    size_t bufferSize = elementSize * capacity;

    uint8_t *buffer = allocator->alloc(
            allocator->context,
            bufferSize,
            align
            );

    if (!buffer) {
        if (allocator->free) allocator->free(allocator->context, array);
        return 0;
    }

    array->count = 0;
    array->capacity = capacity;
    array->elementSize = elementSize;
    array->alignment = align;
    array->expFactor = expansion;
    array->allocator = allocator;
    array->data = buffer;

    *outputPtr = array;
    return 1;
}

void dsa_array_destroy(DsaDynArray **arrayPtr) {
    if (!arrayPtr) return;
    if (!(*arrayPtr)) return;
    
    DsaDynArray *array = (*arrayPtr);
    DsaAllocator *allocator = array->allocator;

    if (allocator && allocator->free) {
        allocator->free(allocator->context, array->data);
        allocator->free(allocator->context, array);
    }

    *arrayPtr = NULL;
}

int dsa_array_safe_get(DsaDynArray *array, size_t index, void **outPtr) {
    if (!array) return 0;
    if (index >= array->count) return 0;

    void *ptr = array->data + index * array->elementSize;
    if (outPtr) *outPtr = ptr;

    return 1;
}

void *dsa_array_get(DsaDynArray *array, size_t index) {
    if (!array) return NULL;
    if (index >= array->count) return NULL;

    return array->data + index * array->elementSize;
}

void *dsa_array_push(DsaDynArray *array) {
    if (!array) return NULL;

    int output = 1;
    if (array->count >= array->capacity) {
        output = dsa_array_expand(array);
    }
    
    if (output) {
        void *ptr = array->data + array->count * array->elementSize;
        array->count += 1;
        return ptr;
    }

    return NULL;
}

int dsa_array_expand(DsaDynArray *array) {
    if (!array) return 0;
    if (array->expFactor < 1.0f) return 0;
    if (!array->allocator) return 0;
    
    size_t newCapacity = (size_t) (array->capacity * array->expFactor); 
    if (newCapacity <= array->capacity) newCapacity = array->capacity + 1;
    
    return dsa_array_resize_internal(array, newCapacity);
}

void *dsa_array_pop(DsaDynArray *array) {
    if (!array) return NULL;
    if (array->count < 1) return NULL;

    void *ptr = array->data + (array->count - 1) * array->elementSize;
    array->count -= 1;
    return ptr;
}

void *dsa_array_peek(DsaDynArray *array) {
    if (!array) return NULL;
    if (array->count < 1) return NULL;

    void *ptr = array->data + (array->count - 1) * array->elementSize;
    return ptr;
}

void *dsa_array_reset(DsaDynArray *array) {
    if (!array) return NULL;

    void *ptr = array->data;
    array->count = 0;
    return ptr;
}

int dsa_array_reserve(DsaDynArray *array, size_t minCapacity) {
    if (!array || !array->allocator || !array->allocator->alloc) return 0;
    if (!minCapacity) return 0;

    if (minCapacity <= array->capacity) return 1;
    
    return dsa_array_resize_internal(array, minCapacity);
}

int dsa_array_resize_internal(DsaDynArray *array, size_t newCapacity) {
    if (!array) return 0;
    DsaAllocator *allocator = array->allocator;
    uint8_t *newBuffer;
    size_t newSizeInBytes = array->elementSize * newCapacity;

    if (allocator->realloc) {
         newBuffer = allocator->realloc(
            allocator->context,
            array->data,
            newSizeInBytes
            );
        
         if (!newBuffer) return 0;
    }
    else {
        if (allocator->alloc) {
            newBuffer = allocator->alloc(
                allocator->context,
                newSizeInBytes,
                array->alignment
                );
        
            if (!newBuffer) return 0;

            memcpy(newBuffer, array->data, array->count * array->elementSize);
            if (allocator->free) allocator->free(allocator->context, array->data);
        }
        else return 0;
    }

    array->data = newBuffer;
    array->capacity = newCapacity;
    return 1;
}
    
#endif
