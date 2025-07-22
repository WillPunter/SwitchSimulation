/*  queue.h */
#ifndef QUEUE_H
#define QUEUE_H

struct queue;
typedef struct queue *queue_t;

/*  Queue API. */
queue_t queue_create(free_func_t free_elem);
void queue_free(queue_t queue);
unsigned int queue_size(queue_t queue);
void queue_enqueue(queue_t queue, void *elem);
void *queue_peek(queue_t queue);
void *queue_dequeue(queue_t queue);

#endif