/* queue.c */

#include "queue.h"
#include "heap.h"
#include <assert.h>

struct queue {
    void **elems;
    unsigned int head;
    unsigned int tail;
    unsigned int capacity;
    free_func_t free_elem;
};

#define DEFAULT_CAPACITY 16

/*  Queue API implementation. */

/*  Queue create - creating a queue simply involves allocating a new queue
    structure, setting default values for its properties and allocating a
    default size element buffer. */
queue_t queue_create(free_func_t free_elem) {
    queue_t queue = (queue_t) malloc(sizeof(struct queue));
    assert(queue);

    queue->head = 0;
    queue->tail = 0;
    queue->capacity = DEFAULT_CAPACITY;

    queue->elems = (void **) malloc(sizeof(void *) * queue->capacity);
    assert(queue->elems);

    return queue;
};

/*  Queue free - to free a queue structure, we first need to iterate through
    all elements from the head to the tail and free them with the function
    provided at creation time. After this, the element buffer / array needs to
    be deallocated. Finally, the queue structure itself needs to be freed. */
void queue_free(queue_t queue) {
    int i = queue->head;

    while (i != queue->tail) {
        queue->free_elem(queue->elems[i]);

        i = (i + 1) % queue->capacity;
    };

    free((void *) queue->elems);
    free((void *) queue);
};

/*  Queue size - the size is the positive distance from the head pointer to the
    tail pointer. If the tail pointer is greater than the head pointer then
    this is simply just tail - head, but if the tail pointer has wrapped back
    around, then it is capacity - head + tail. */
unsigned int queue_size(queue_t queue) {
    return (queue->head < queue->tail) ?
        queue->tail - queue->head :
        queue->capacity - queue->head + queue->tail;
};

void queue_enqueue(queue_t queue, void *elem) {

};

void *queue_peek(queue_t queue) {

};

void *queue_dequeue(queue_t queue) {

};