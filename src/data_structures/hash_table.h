/*  hash_table.h */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "heap.h"

struct hash_table;
typedef struct hash_table *hash_table_t;

typedef unsigned int hash_t;

typedef hash_t (*hash_func_t)(void *);

/*  Hash table API. */
hash_table_t hash_table_create(
    hash_func_t hash_func,
    comparator_func_t key_compare,
    free_func_t free_key,
    free_func_t free_val
);

void hash_table_free(hash_table_t hash_table);
void *hash_table_lookup(hash_table_t hash_table, void *key);
void hash_table_insert(hash_table_t hash_table, void *key, void *value);
void hash_table_remove(hash_table_t hash_table, void *key);
int hash_table_size(hash_table_t hash_table);

#endif