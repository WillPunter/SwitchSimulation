/*  test_heap.c */

#include "./../test.h"
#include "heap.h"
#include <assert.h>

/*  Test structures for generic heap. */
struct elem {
    int data;
};

typedef struct elem *elem_t;

static comparison_t elem_compare(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    elem_t elem_lhs = (elem_t) lhs;
    elem_t elem_rhs = (elem_t) rhs;

    if (elem_lhs->data < elem_rhs->data) {
        return LT;
    } else if (elem_lhs->data > elem_rhs->data) {
        return GT;
    } else {
        return EQ;
    };
};

static elem_t elem_create(int data) {
    elem_t elem = (elem_t) malloc(sizeof(struct elem));
    elem->data = data;
    assert(elem);

    return elem;
};

static void elem_free(void *elem_ptr) {
    assert(elem_ptr);
    free(elem_ptr);
};

/*  Unit tests. */
DEFINE_TEST(test_heap_create_destroy)
    heap_t heap = heap_create(elem_compare, elem_free);
    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_insert_1)
    heap_t heap = heap_create(elem_compare, elem_free);

    ASSERT_EQ(0, heap_size(heap))

    elem_t elem_1 = elem_create(10);
    heap_insert(heap, elem_1);

    ASSERT_EQ(1, heap_size(heap))

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_pop_1)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(10);

    heap_insert(heap, elem_1);
    
    elem_t elem_pop = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_pop->data, elem_1->data)
    free((void *) elem_pop);

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_pop_2)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(10);
    elem_t elem_2 = elem_create(20);

    heap_insert(heap, elem_1);
    heap_insert(heap, elem_2);

    elem_t elem_pop_1 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_1->data, elem_pop_1->data)
    free((void *) elem_pop_1);

    elem_t elem_pop_2 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_2->data, elem_pop_2->data)
    free((void *) elem_pop_2);

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_pop_3)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(20);
    elem_t elem_2 = elem_create(10);

    heap_insert(heap, elem_1);
    heap_insert(heap, elem_2);

    elem_t elem_pop_1 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_2->data, elem_pop_1->data)
    free((void *) elem_pop_1);

    elem_t elem_pop_2 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_1->data, elem_pop_2->data)
    free((void *) elem_pop_2);

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_pop_4)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(4);
    elem_t elem_2 = elem_create(2);
    elem_t elem_3 = elem_create(3);
    elem_t elem_4 = elem_create(1);

    heap_insert(heap, elem_1);
    heap_insert(heap, elem_2);
    heap_insert(heap, elem_3);

    elem_t elem_pop_1 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_2->data, elem_pop_1->data)
    free((void *) elem_pop_1);
    
    elem_t elem_pop_2 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_3->data, elem_pop_2->data)
    free((void *) elem_pop_2);

    heap_insert(heap, elem_4);

    elem_t elem_pop_3 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_4->data, elem_pop_3->data);
    free((void *) elem_pop_3);

    elem_t elem_pop_4 = (elem_t) heap_pop_min(heap);
    ASSERT_EQ(elem_1->data, elem_pop_4->data);
    free((void *) elem_pop_4);

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_size_1)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(3);
    elem_t elem_2 = elem_create(2);
    elem_t elem_3 = elem_create(1);

    ASSERT_EQ(0, heap_size(heap))

    heap_insert(heap, elem_1);
    ASSERT_EQ(1, heap_size(heap))

    heap_insert(heap, elem_2);
    ASSERT_EQ(2, heap_size(heap))

    elem_t elem_pop_1 = heap_pop_min(heap);
    free((void *) elem_pop_1);
    ASSERT_EQ(1, heap_size(heap))

    heap_insert(heap, elem_3);
    ASSERT_EQ(2, heap_size(heap))

    heap_free(heap);
END_TEST

DEFINE_TEST(test_heap_min_1)
    heap_t heap = heap_create(elem_compare, elem_free);

    elem_t elem_1 = elem_create(3);
    elem_t elem_2 = elem_create(2);

    heap_insert(heap, elem_1);
    elem_t elem_1_min = (elem_t) heap_min(heap);
    ASSERT_EQ(elem_1->data, elem_1_min->data)

    heap_insert(heap, elem_2);
    elem_t elem_2_min = (elem_t) heap_min(heap);
    ASSERT_EQ(elem_2->data, elem_2_min->data)

    elem_t elem_1_pop = heap_pop_min(heap);
    free((void *) elem_1_pop);

    elem_t elem_3_min = heap_min(heap);
    ASSERT_EQ(elem_1->data, elem_3_min->data)

    heap_free(heap);
END_TEST

REGISTER_TESTS(
    test_heap_create_destroy,
    test_heap_insert_1,
    test_heap_pop_1,
    test_heap_pop_2,
    test_heap_pop_3,
    test_heap_pop_4,
    test_heap_size_1,
    test_heap_min_1
)