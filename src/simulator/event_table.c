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

void event_table_free();
void event_table_register_event(
    event_id_t evt_id,
    callback_func_t callback,
    free_func_t free_arg
);

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