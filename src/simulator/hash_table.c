/*  hash_table.c */

#include "hash_table.h"
#include <assert>

#define DEFAULT_CAPACITY 32

/* Hash table structure with linear probing to handle hash collisions. */
struct hash_table_entry {
    void *key;
    void *value;
    struct hash_table_entry *next;
};

typedef struct hash_table_entry *hash_table_entry_t;

struct hash_table {
    hash_table_entry_t **entries;
    unsigned int size;
    unsigned int capacity;
    hash_func_t hash_func;
    free_func_t free_key;
    free_func_t free_val;
};

/*  Implement API functions. */

/*  Create hash table - creating a hash table involves allocating a hash table
    structure, then allocating a default number of entry pointers and finally
    setting the structure properties (e.g. the hash function, element free
    function, size and capacity) before returning. */
hash_table_t hash_table_create(
    hash_func_t hash_func,
    free_func_t free_key,
    free_func_t free_val
) {
    assert(hash_func);
    assert(free_elem);

    hash_table_t hash_table = (hash_table_t) malloc(sizeof(struct hash_table));
    assert(hash_table);

    hash_table->entries =
        (hash_table_entry **) malloc(
            sizeof((struct hash_table_entry) *) * DEFAULT_CAPACITY
        );
    
    assert(hash_table->entries);

    hash_table->size = 0;
    hash_table->capacity = DEFAULT_CAPACITY;
    hash_table->hash_func = hash_func;
    hash_table->free_key = free_key;
    hash_table->free_val = free_val;

    return hash_table;
};

void hash_table_free(hash_table_t hash_table);

void *hash_table_lookup(hash_table_t hash_table, void *key);
void hash_table_insert(hash_table_t hash_table, void *key, void *value);
void hash_table_remove(hash_table_t hash_table, void *key);