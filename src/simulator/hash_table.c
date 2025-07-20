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
    comparator_func_t key_compare;
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
    comparator_func_t key_compare,
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
    hash_table->key_compare = key_compare;
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
    assert(hash_table);
    assert(hash_table->entries);
    assert(hash_table->free_key);
    assert(hash_table->free_val);

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

/*  Hash table lookup - this involves computing the hash of the key modulo
    capacity and then, if a list of elements is present, searching the linked
    list for an element that is equal to the key - only in this case does the
    function return the value as opposed to NULL. */
void *hash_table_lookup(hash_table_t hash_table, void *key) {
    assert(hash_table);
    assert(hash_table->entries);
    assert(hash_table->key_compare);
    assert(key);
    
    hash_t hash = hash_table->hash_func(key) % hash_table->capacity;

    if (hash_table->entries[hash]) {
        hash_table_entry_t curr = hash_table->entries[hash];

        while (curr) {
            comparison_t compare = hash_table->key_compare(key, curr->key);

            if (compare == EQ) {
                return curr->val;
            }

            curr = curr->next;
        }
    }

    return NULL;
};

/*  Hash table insert - inserting into a hash table first means checking that
    the hash table has the capacity to store the new entry - ideally each
    element should have its own unique location in the table - chaining should
    be avoided where possible to keep the average case lookup time down (even
    if not asymptotically, of course).
    
    If the new size will be greater than the capacity, we need to resize.
    Unfortunately, the change of underyling array size means that the existing
    elements may not be in the correct locations (since their locations are
    computed as hash mod array size), and hence need to be extracted and
    reinserted.
    
    Note that if the key already exists - or something that evaluates to the
    same under the comparator function - then we simply replace the existing
    entry and do not modify the size. */
void hash_table_insert(hash_table_t hash_table, void *key, void *value) {
    assert(hash_table);
    assert(key);
    assert(value);

    if (hash_table_lookup(hash_table, key) != NULL) {
        /*  The key is already in the hash table, we should update the existing
            entry rather than creating a new one. */
        hash_t hash = hash_table->hash_func(key) % hash_table->capacity;

        hash_table_entry_t curr = hash_table->entries[hash];

        while (curr) {
            if (hash_table->key_compare(key, curr->key) == EQ) {
                curr->key = key;
                curr->val = value;
                return;
            }

            curr = curr->next;
        }
    } else {
        /*  We are adding a new element so size will increase. We must resize
            the underlying array if the size has outgrown the capacity. */
        if (hash_table->size + 1 > hash_table->capacity) {
            /*  Allocate buffer for storing elements temporarily before transfer
                to new, larger array. */
            struct hash_table_entry *elems =
                (struct hash_table_entry *) malloc(
                    sizeof(struct hash_table_entry) * hash_table->capacity
                );
            
            /*  Copy key and value pairs into new buffer. Delete existing entries
                and only store their keys and values. */
            int i;
            int current_elem = 0;

            for (i = 0; i < hash_table->capacity; i++) {
                if (hash_table->entries[i]) {
                    hash_table_entry_t curr = hash_table->entries[i];

                    while (curr) {
                        elems[current_elem].key = curr->key;
                        elems[current_elem].val = curr->val;
                        current_elem++;
                        curr = curr->next;
                        free(curr);
                    }
                }
            }

            /*  Free existing entry array. */
            hash_table->size = 0;
            free(hash_table->entries);
            hash_table->capacity = 2 * hash_table->capacity;

            hash_table->entries = (hash_table_entry_t *) malloc(
                sizeof(hash_table_entry_t) * hash_table->capacity
            );

            /*  Insert elements back in to hash table. */
            for (i = 0; i < current_elem; i++) {
                hash_table_insert(hash_table, elems[i].key, elems[i].val);
            }

            free(elems);
        }

        hash_table->size ++;

        /*  Allocate new entry structure and insert in place. */
        hash_table_entry_t new_entry =
            (hash_table_entry_t) malloc(sizeof(struct hash_table_entry));
        
        new_entry->key = key;
        new_entry->val = value;
        new_entry->next = NULL;

        hash_t hash = hash_table->hash_func(key) % hash_table->capacity;

        if (hash_table->entries[hash]) {
            /*  Existing linked list / chain at location - insert at end. */
            hash_table_entry_t curr = hash_table->entries[hash];

            while (curr->next) {
                curr = curr->next;
            }

            curr->next = new_entry;
        } else {
            /*  No linked list / chain at location yet - start one. */
            hash_table->entries[hash] = new_entry;
        }
    }
};

/*  Hash table remove - to remove an element from a hash table, simply look up
    it's location by checking the entries that hash to the same location as the
    provided key. Free the key and value structures first, then the entry
    structure, fixing the linked list by modifying the previous element. */
void hash_table_remove(hash_table_t hash_table, void *key) {
    hash_t hash = hash_table->hash_func(key) % hash_table->capacity;

    if (hash_table->entries[hash]) {
        hash_table_entry_t curr = hash_table->entries[hash];
        hash_table_entry_t *ptr = &hash_table->entries[hash];

        while (curr) {
            if (hash_table->key_compare(key, curr->key) == EQ) {
                hash_table->free_key(curr->key);
                hash_table->free_val(curr->val);

                *ptr = curr->next;
                free((void *) curr);

                return;
            }

            if (curr->next) {
                ptr = &curr->next;
            }
            
            curr = curr->next;
        }
    }
};