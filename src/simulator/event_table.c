/*  event_table.c */
#include "event_table.h"
#include "hash_table.h"
#include <assert.h>

struct event_table {
    hash_table_t hash_table;
};

struct event_entry_key {
    event_id_t key;
};

typedef struct event_entry_key *event_entry_key_t;

struct event_entry_val {
    callback_func_t callback;
    free_func_t free_arg;
};

typedef struct event_entry_val *event_entry_val_t;

typedef struct event_entry *event_entry_t;

/*  Forward declare helper functions. */
static hash_t event_entry_hash(void *evt_key);
static comparison_t event_entry_key_compare(void *lhs, void *rhs);
static void event_entry_key_free(void *key);
static void event_entry_val_free(void *val);

/*  Event table API implementation. */
event_table_t event_table_create() {
    event_table_t event_table =
        (event_table_t) malloc(sizeof(struct event_table));
    
    event_table->hash_table = hash_table_create(
        event_entry_hash,
        event_entry_key_compare,
        event_entry_key_free,
        event_entry_val_free
    );

    return event_table;
};

/*  Free event table - this simply just involves freeing the underlying hash
    table and then freeing the event table structure. Note that the hash table
    free functions invokes the free functions for the keys and values given
    to the hash_table at creation time. */
void event_table_free(event_table_t event_table) {
    assert(event_table);
    assert(event_table->hash_table);

    hash_table_free(event_table->hash_table);
    free(event_table);
};

/*  Register event - this adds a mapping from an event id to a callback and
    argument freeing function. To do this, we need to first allocate an event
    table key and value pair. */
void event_table_register_event(
    event_table_t event_table,
    event_id_t evt_id,
    callback_func_t callback,
    free_func_t free_arg
) {
    event_entry_key_t key = malloc(sizeof(struct event_entry_key));
    assert(key);

    event_entry_val_t val = malloc(sizeof(struct event_entry_val));
    assert(val);

    hash_table_insert(event_table->hash_table, (void *) key, (void *) val);
};

/*  Event table lookup - look a record up by event id and then obtain the
    corresponding callback and free argument functions. */
void event_table_lookup(
    event_table_t event_table,
    event_id_t evt_id,
    callback_func_t *callback_out,
    free_func_t *free_arg_out
) {
    /*  Create temporary key structure on the stack for looking up. */
    struct event_entry_key lookup_key;
    lookup_key.key = evt_id;

    /*  Look up element with key in the stack. */
    void *lookup_val = hash_table_lookup(event_table->hash_table, &lookup_key);

    if (lookup_val) {
        event_entry_val_t val = (event_entry_val_t) lookup_val;
        *callback_out = val->callback;
        *free_arg_out = val->free_arg;
    } else {
        *callback_out = 0;
        *free_arg_out = 0;
    }
};

/*  Helper function implementations / definitions. */
static hash_t event_entry_hash(void *evt_key) {
    assert(evt_key);

    event_entry_key_t key = evt_key;

    return key->key;
};

static comparison_t event_entry_key_compare(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    event_entry_key_t lhs_key = (event_entry_key_t) lhs;
    event_entry_key_t rhs_key = (event_entry_val_t) rhs;

    if (lhs_key->key < rhs_key->key) {
        return LT;
    } else if (lhs_key->key > rhs_key->key) {
        return GT;
    } else {
        return EQ;
    }
};

static void event_entry_key_free(void *key) {
    assert(key);
    free(key);
}

static void event_entry_val_free(void *val) {
    assert(val);
    free(val);
}