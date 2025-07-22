/*  test_hash_table.c */

#include "./../test.h"
#include "hash_table.h"
#include <assert.h>
#include <malloc.h>

struct elem_key {
    int data;
};

typedef struct elem_key *elem_key_t;

struct elem_val {
    char data;
};

typedef struct elem_val *elem_val_t;

static hash_t elem_key_hash(void *key_ptr) {
    assert(key_ptr);

    elem_key_t key = (elem_key_t) key_ptr;

    return key->data;
};

static comparison_t elem_key_compare(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    elem_key_t elem_lhs = (elem_key_t) lhs;
    elem_key_t elem_rhs = (elem_key_t) rhs;

    if (elem_lhs->data < elem_rhs->data) {
        return LT;
    } else if (elem_lhs->data > elem_rhs->data) {
        return GT;
    } else {
        return EQ;
    };
};

static elem_key_t elem_key_create(int data) {
    elem_key_t key = (elem_key_t) malloc(sizeof(struct elem_key));
    key->data = data;
    return key;
}

static void elem_key_free(void *key_ptr) {
    assert(key_ptr);
    free(key_ptr);
};

static elem_val_t elem_val_create(char data) {
    elem_val_t val = (elem_val_t) malloc(sizeof(struct elem_val));
    val->data = data;
    return val;
}

static void elem_val_free(void *val_ptr) {
    assert(val_ptr);
    free(val_ptr);
}

DEFINE_TEST(test_hash_table_create_destroy)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_size_1)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    elem_key_t key_1 = elem_key_create(100);
    elem_val_t val_1 = elem_val_create('A');

    ASSERT_EQ(0, hash_table_size(hash_table))

    hash_table_insert(hash_table, (void *) key_1, (void *) val_1);
    
    ASSERT_EQ(1, hash_table_size(hash_table));

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_resizes_1)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    int i;
    for (i = 0; i < 1500; i++) {
        elem_key_t key = elem_key_create(i);
        elem_val_t val = elem_val_create('X');
        hash_table_insert(hash_table, (void *) key, (void *) val);
    };

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_lookup_1)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    elem_key_t key_1 = elem_key_create(50);
    elem_val_t val_1 = elem_val_create('A');
    hash_table_insert(hash_table, (void *) key_1, (void *) val_1);

    elem_val_t val_lookup = (elem_val_t) hash_table_lookup(hash_table, key_1);

    ASSERT_EQ(val_1->data, val_lookup->data);

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_lookup_2)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    elem_key_t keys[1500];

    int i;
    for (i = 0; i < 1500; i++) {
        keys[i] = elem_key_create(i);
        elem_val_t val = elem_val_create((char) i);
        hash_table_insert(hash_table, (void *) keys[i], (void *) val);
    };

    for (i = 0; i < 1500; i++) {
        elem_val_t val_lookup = hash_table_lookup(hash_table, keys[i]);
        ASSERT_EQ((char) i, val_lookup->data);
    };

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_remove_1)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    elem_key_t key = elem_key_create(10);
    elem_val_t val = elem_val_create('B');

    hash_table_insert(hash_table, key, val);

    elem_val_t val_lookup_1 = (elem_val_t) hash_table_lookup(hash_table, key);
    ASSERT_EQ(val->data, val_lookup_1->data);

    hash_table_remove(hash_table, key);

    /*  key is freed here. */

    elem_key_t key_2 = elem_key_create(10);
    elem_val_t val_lookup_2 = (elem_val_t) hash_table_lookup(hash_table, key_2);
    ASSERT_EQ(NULL, val_lookup_2);
    free(key_2);

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_remove_2)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    elem_key_t key_1 = elem_key_create(1);
    elem_key_t key_2 = elem_key_create(2);
    elem_key_t key_3 = elem_key_create(3);

    elem_val_t val_1 = elem_val_create('A');
    elem_val_t val_2 = elem_val_create('B');
    elem_val_t val_3 = elem_val_create('C');

    hash_table_insert(hash_table, (void *) key_1, (void *) val_1);
    hash_table_insert(hash_table, (void *) key_2, (void *) val_2);
    hash_table_insert(hash_table, (void *) key_3, (void *) val_3);

    elem_val_t val_lookup = hash_table_lookup(hash_table, key_1);
    ASSERT_TRUE(val_lookup)
    ASSERT_EQ(val_1->data, val_lookup->data)

    val_lookup = hash_table_lookup(hash_table, key_2);
    ASSERT_TRUE(val_lookup)
    ASSERT_EQ(val_2->data, val_lookup->data)

    val_lookup = hash_table_lookup(hash_table, key_3);
    ASSERT_TRUE(val_lookup)
    ASSERT_EQ(val_3->data, val_lookup->data)

    hash_table_remove(hash_table, key_1);
    elem_key_t key_lookup = elem_key_create(1);
    val_lookup = hash_table_lookup(hash_table, key_lookup);
    ASSERT_FALSE(val_lookup)
    free(key_lookup);

    hash_table_remove(hash_table, key_2);
    key_lookup = elem_key_create(2);
    val_lookup = hash_table_lookup(hash_table, key_lookup);
    ASSERT_FALSE(val_lookup)
    free(key_lookup);

    hash_table_remove(hash_table, key_3);
    key_lookup = elem_key_create(3);
    val_lookup = hash_table_lookup(hash_table, key_lookup);
    ASSERT_FALSE(val_lookup)
    free(key_lookup);

    hash_table_free(hash_table);
END_TEST

DEFINE_TEST(test_hash_table_size_2)
    hash_table_t hash_table = hash_table_create(
        elem_key_hash,
        elem_key_compare,
        elem_key_free,
        elem_val_free
    );

    ASSERT_EQ(0, hash_table_size(hash_table))

    elem_key_t key_1 = elem_key_create(1);
    elem_key_t key_2 = elem_key_create(2);
    elem_key_t key_3 = elem_key_create(3);

    elem_val_t val_1 = elem_val_create('A');
    elem_val_t val_2 = elem_val_create('B');
    elem_val_t val_3 = elem_val_create('C');

    hash_table_insert(hash_table, (void *) key_1, (void *) val_1);
    ASSERT_EQ(1, hash_table_size(hash_table))

    hash_table_insert(hash_table, (void *) key_2, (void *) val_2);
    ASSERT_EQ(2, hash_table_size(hash_table))

    hash_table_remove(hash_table, key_1);
    ASSERT_EQ(1, hash_table_size(hash_table))

    hash_table_insert(hash_table, (void *) key_3, (void *) val_3);
    ASSERT_EQ(2, hash_table_size(hash_table))

    hash_table_remove(hash_table, key_2);
    ASSERT_EQ(1, hash_table_size(hash_table));

    hash_table_remove(hash_table, key_3);
    ASSERT_EQ(0, hash_table_size(hash_table));

    hash_table_free(hash_table);
END_TEST

REGISTER_TESTS(
    test_hash_table_create_destroy,
    test_hash_table_size_1,
    test_hash_table_resizes_1,
    test_hash_table_lookup_1,
    test_hash_table_lookup_2,
    test_hash_table_remove_1,
    test_hash_table_remove_2,
    test_hash_table_size_2
)