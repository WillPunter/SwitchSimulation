/*  test_queue.c */

#include "./../test.h"
#include "queue.h"
#include <assert.h>

/*  Test structure for element. */
struct elem {
    int data;
};

typedef struct elem *elem_t;

static elem_t elem_create(int data) {
    elem_t elem = (elem_t) malloc(sizeof(struct elem));
    assert(elem);

    elem->data = data;

    return elem;
};

static void elem_free(void *elem) {
    free(elem);
};

/*  Tests. */

DEFINE_TEST(test_queue_create_free)
    queue_t queue = queue_create(elem_free);
    assert(queue);
    queue_free(queue);
END_TEST

DEFINE_TEST(test_queue_enqueue_dequeue)
    queue_t queue = queue_create(elem_free);
    assert(queue);

    elem_t elem_1 = elem_create(5);
    queue_enqueue(queue, (void *) elem_1);
    elem_t elem_1_dequeue = (elem_t) queue_dequeue(queue);

    ASSERT_EQ(elem_1_dequeue->data, elem_1->data)

    elem_free(elem_1_dequeue);

    queue_free(queue);
END_TEST

DEFINE_TEST(test_queue_memory_free)
    queue_t queue = queue_create(elem_free);
    assert(queue);

    elem_t elem_1 = elem_create(5);
    queue_enqueue(queue, (void *) elem_1);

    queue_free(queue);
END_TEST

DEFINE_TEST(test_queue_enqueue_dequeue_2)
    queue_t queue = queue_create(elem_free);
    assert(queue);

    elem_t elem_1 = elem_create(1);
    elem_t elem_2 = elem_create(2);
    elem_t elem_3 = elem_create(3);
    elem_t elem_4 = elem_create(4);
    elem_t elem_5 = elem_create(5); 
    
    queue_enqueue(queue, (void *) elem_1);
    queue_enqueue(queue, (void *) elem_2);
    queue_enqueue(queue, (void *) elem_3);

    elem_t elem_1_dequeue = (elem_t) queue_dequeue(queue);
    ASSERT_EQ(elem_1_dequeue->data, elem_1->data)

    queue_enqueue(queue, (void *) elem_4);

    elem_t elem_2_dequeue = (elem_t) queue_dequeue(queue);
    ASSERT_EQ(elem_2_dequeue->data, elem_2->data);

    elem_t elem_3_dequeue = (elem_t) queue_dequeue(queue);
    ASSERT_EQ(elem_3_dequeue->data, elem_3->data);

    elem_t elem_4_dequeue = (elem_t) queue_dequeue(queue);
    ASSERT_EQ(elem_4_dequeue->data, elem_4->data);

    queue_enqueue(queue, elem_5);

    elem_t elem_5_dequeue = (elem_t) queue_dequeue(queue);
    ASSERT_EQ(elem_5_dequeue->data, elem_5->data);

    elem_free(elem_1);
    elem_free(elem_2);
    elem_free(elem_3);
    elem_free(elem_4);
    elem_free(elem_5);

    queue_free(queue);
END_TEST

DEFINE_TEST(test_queue_peek)
    queue_t queue = (queue_t) queue_create(elem_free);

    queue_free(queue);
END_TEST

REGISTER_TESTS(
    test_queue_create_free,
    test_queue_enqueue_dequeue,
    test_queue_memory_free,
    test_queue_enqueue_dequeue_2
)