#ifndef DSA_HASHTABLE_H
#define DSA_HASHTABLE_H

typedef struct dsa_hashtable DsaHashtable;
typedef struct dsa_table_entry DsaTableEntry;

#include "dataStructures/allocators.h"
#include "dataStructures/stringview.h"

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
//int dsa_hashtable_copy(const DsaHashtable *src, DsaHashtable *dest);
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

       


#endif
