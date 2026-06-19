#ifndef DSA_HASHTABLE_H
#define DSA_HASHTABLE_H

#include <string.h>
typedef struct dsa_hashtable DsaHashtable;
typedef struct dsa_table_entry DsaTableEntry;

#include "dataStructures/allocators.h"
#include "dataStructures/stringview.h"

enum DSA_ENTRY_STATE {
    DSA_ENTRY_EMPTY,
    DSA_ENTRY_OCCUPIED,
    DSA_ENTRY_TOMBSTONE
};

struct dsa_table_entry {
    uint64_t hash;
    DsaStringView key;
    void *value;
    uint8_t state;
};

struct dsa_hashtable {
    DsaTableEntry *buckets;
    size_t count;
    size_t capacity;
    float expFactor;
    DsaAllocator *allocator;
};


int dsa_hashtable_new(
        DsaAllocator *allocator,
        size_t capacity, float expansion,
        DsaHashtable **outputPtr);

uint64_t dsa_hash_str_djb2(DsaStringView *key);


int dsa_hashtable_put(DsaHashtable *table, DsaStringView *key, void *value);
void *dsa_hashtable_get(DsaHashtable *table, DsaStringView *key);
void *dsa_hashtable_remove(DsaHashtable *table, DsaStringView *key);

int dsa_hashtable_expand(DsaHashtable *table);
void dsa_hashtable_destroy(DsaHashtable **tablePtr);
int dsa_hashtable_copy(const DsaHashtable *src, DsaHashtable *dest);
int dsa_table_rehash(DsaHashtable *table, DsaTableEntry* newBuckets, size_t newCapacity); 
//int dsa_hashtable_rescue(
//        DsaAllocator *outputAllocator, 
//        DsaHashtable *origin, 
//        DsaHashtable **outputPtr);
//implementacao nao necessaria

int dsa_hashtable_insert_internal(
        DsaHashtable *table, 
        uint64_t hash,
        DsaStringView *key,
        void *value);

void *dsa_hashtable_find_internal(
        DsaHashtable *table,
        uint64_t hash,
        DsaStringView *key);

int dsa_hashtable_new(
        DsaAllocator *allocator, 
        size_t capacity, float expansion, 
        DsaHashtable **outputPtr) {

    if (!capacity || expansion <= 1.0f) return 0;
    if (!outputPtr || !allocator || !allocator->alloc) return 0;

    DsaHashtable *table = allocator->alloc(
            allocator->context,
            sizeof(DsaHashtable),
            _Alignof(DsaHashtable)
            );

    if (!table) return 0;

    DsaTableEntry *buckets = allocator->alloc(
            allocator->context,
            sizeof(DsaTableEntry) * capacity,
            _Alignof(DsaTableEntry)
            );
    
    if (!buckets) {
        if (allocator->free) allocator->free(allocator->context, table);
        return 0;
    }

    memset(buckets, 0, sizeof(DsaTableEntry) * capacity);

    table->count = 0;
    table->capacity = capacity;
    table->expFactor = expansion;
    table->allocator = allocator;
    table->buckets = buckets;

    *outputPtr = table;
    return 1;
}

int dsa_hashtable_put(DsaHashtable *table, DsaStringView *key, void *value) {
    if (!table || !key || !key->text || !key->size) return 0;

    if (table->count >= 0.7 * table->capacity) {
        int output = dsa_hashtable_expand(table);
        
        if (!output) return 0;
    }

    uint64_t hash = dsa_hash_str_djb2(key);
    
    return dsa_hashtable_insert_internal(table, hash, key, value);
}

int dsa_hashtable_insert_internal(
        DsaHashtable *table, 
        uint64_t hash, 
        DsaStringView *key,
        void *value) {

    size_t index = hash % table->capacity, startIndex = index;
    int firstDeletedIndex = -1;
    
    DsaTableEntry *entry = NULL;

    while (1) {
        entry = &table->buckets[index];

        if (entry->state == DSA_ENTRY_EMPTY) {
            if (firstDeletedIndex != -1) entry = &table->buckets[firstDeletedIndex];

            entry->hash = hash;
            entry->key = *key;
            entry->value = value;
            entry->state = DSA_ENTRY_OCCUPIED;
            table->count += 1;

            return 1;
        }

        if (entry->state == DSA_ENTRY_TOMBSTONE) {
            if (firstDeletedIndex == -1) firstDeletedIndex = index;
        }

        if (entry->state == DSA_ENTRY_OCCUPIED) {
            if (entry->hash == hash && dsa_str_equals(&entry->key, key)) {
                entry->value = value;
                return 1;
            }
        }

        index = (index + 1) % table->capacity;

        if (index == startIndex) {
            if (firstDeletedIndex != -1) {
                entry = &table->buckets[firstDeletedIndex];
            
                entry->hash = hash;
                entry->key = *key;
                entry->value = value;
                entry->state = DSA_ENTRY_OCCUPIED;
                table->count += 1;

                return 1;
            }
            return 0;
        }
    }
}

void *dsa_hashtable_get(DsaHashtable *table, DsaStringView *key) {
    if (!table || !key || !key->text || !key->size) return NULL;

    uint64_t hash = dsa_hash_str_djb2(key);

    return dsa_hashtable_find_internal(table, hash, key);
}

void *dsa_hashtable_find_internal(
        DsaHashtable *table,
        uint64_t hash,
        DsaStringView *key) {

    size_t index = hash % table->capacity, startIndex = index;

    DsaTableEntry *entry = NULL;

    while (1) {
        entry = &table->buckets[index];

        if (entry->state == DSA_ENTRY_OCCUPIED) {
            if (entry->hash == hash && dsa_str_equals(&entry->key, key)) {
                return entry->value;
            }
        }

        if (entry->state == DSA_ENTRY_EMPTY) return NULL;
        index = (index + 1) % table->capacity;
        if (index == startIndex) return NULL;
    }
}

void *dsa_hashtable_remove(DsaHashtable *table, DsaStringView *key) {
    if (!table || !key || !key->text || !key->size) return NULL;

    uint64_t hash = dsa_hash_str_djb2(key);
    size_t index = hash % table->capacity, startIndex = index;

    DsaTableEntry *entry = NULL;
    void *entryValue = NULL;

    while (1) {
        entry = &table->buckets[index];

        if (entry->state == DSA_ENTRY_OCCUPIED) {
            if (entry->hash == hash && dsa_str_equals(&entry->key, key)) {
                entryValue = entry->value;
                entry->state = DSA_ENTRY_TOMBSTONE;
                table->count -= 1;
                return entryValue;
            }
        }

        if (entry->state == DSA_ENTRY_EMPTY) return NULL;
        index = (index + 1) % table->capacity;
        if (index == startIndex) return NULL;
    }
}

int dsa_hashtable_expand(DsaHashtable *table) {
    if (!table || table->expFactor <= 1.0f) return 0;
    if (!table->allocator || !table->allocator->alloc) return 0;

    DsaAllocator *allocator = table->allocator;
    size_t newSize = table->expFactor * table->capacity;
    
    DsaTableEntry* newBuckets = allocator->alloc(        
            allocator->context,
            (size_t) (sizeof(DsaTableEntry) * newSize),
            _Alignof(DsaTableEntry)
            );

    if (!newBuckets) return 0;
    memset(newBuckets, 0, sizeof(DsaTableEntry) * newSize);
    return dsa_table_rehash(table, newBuckets, newSize);
}

int dsa_table_rehash(DsaHashtable *table, DsaTableEntry *newBuckets, size_t newCapacity) {
    DsaTableEntry *entry = NULL;
     
    DsaTableEntry *oldBuckets = table->buckets;
    size_t oldCapacity = table->capacity, oldCount = table->count;
    table->buckets = newBuckets;
    table->capacity = newCapacity;
    table->count = 0;

    int output = 1;
    for (size_t i=0; i<oldCapacity; i++) {
        entry = &oldBuckets[i];

        if (entry->state == DSA_ENTRY_OCCUPIED) {
            output = dsa_hashtable_insert_internal(
                    table, 
                    entry->hash, 
                    &entry->key,
                    entry->value
                    );
        }
    }

    if (!output) {
        table->buckets = oldBuckets;
        table->capacity = oldCapacity;
        table->count = oldCount;

        if (table->allocator->free) table->allocator->free(
                table->allocator->context,
                newBuckets
                );

        return 0;
    }

    if (table->allocator->free) table->allocator->free(
            table->allocator->context,
            oldBuckets
            );

    return 1;
}
            
uint64_t dsa_hash_str_djb2(DsaStringView *key) {
    uint64_t hash = 5381;

    for (size_t i=0; i<key->size; i++) {
        char c = key->text[i];

        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}
        


#endif
