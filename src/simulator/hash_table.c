/*  hash_table.c */

#include "hash_table.h"
#include <assert.h>
#include <stddef.h>

#define DEFAULT_CAPACITY 32

/* Hash table structure with linear probing to handle hash collisions. */
struct hash_table_entry {
    void *key;
    void *val;
    struct hash_table_entry *next;
};

typedef struct hash_table_entry *hash_table_entry_t;

struct hash_table {
    hash_table_entry_t *entries;
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
    assert(free_key);
    assert(free_val);

    hash_table_t hash_table = (hash_table_t) malloc(sizeof(struct hash_table));
    assert(hash_table);

    hash_table->entries =
        (hash_table_entry_t *) malloc(
            sizeof(hash_table_entry_t) * DEFAULT_CAPACITY
        );
    
    assert(hash_table->entries);

    memset(
        (void *) hash_table->entries,
        0,
        sizeof(hash_table_entry_t) * DEFAULT_CAPACITY
    );

    hash_table->size = 0;
    hash_table->capacity = DEFAULT_CAPACITY;
    hash_table->hash_func = hash_func;
    hash_table->free_key = free_key;
    hash_table->free_val = free_val;

    return hash_table;
};

/*  Free hash table - to free the memory of a hash table, we first need to
    iterate through the array of entry pointers, and free all of the elements
    in each chain, including the key they store, the value they store and the
    entries themselves. Then we free the entries array, and finally we free
    the table itself. */
void hash_table_free(hash_table_t hash_table) {
    int i;
    for (i = 0; i < hash_table->capacity; i++) {
        hash_table_entry_t *entry = hash_table->entries[i];

        if (entry) {
            hash_table_entry_t prev = NULL;
            hash_table_entry_t curr = entry;

            while (curr) {
                if (curr->key) {
                    hash_table->free_key(curr->key);
                }

                if (curr->val) {
                    hash_table->free_val(curr->val);
                }

                prev = curr;
                curr = prev->next;

                free(prev);
            }

            hash_table->entries[i] = NULL;
        }
    }

    free(hash_table->entries);

    free(hash_table);
};

void *hash_table_lookup(hash_table_t hash_table, void *key);
void hash_table_insert(hash_table_t hash_table, void *key, void *value);
void hash_table_remove(hash_table_t hash_table, void *key);