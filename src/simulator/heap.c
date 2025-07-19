/*  heap.c
    
    Implementation of generic binary heap. */

#include "heap.h"
#include <assert.h>

static const DEFAULT_CAPACITY = 16;

/*  Heap structure. */
struct heap {
    void ** elems;
    unsigned int capacity;
    unsigned int size;
    comparator_func_t comparator;
    free_func_t free_elem;
};

/*  Forward declare helper functions. */

/*  Heap API implementation. */

/*  Create a heap - allocate space for the heap structure, then space for the
    array and set the fields, with capacity and size as their default values
    and the comparator and free_elem functions set to the function
    arguments. */
heap_t heap_create(comparator_func_t comparator, free_func_t free_elem) {
    assert(comparator);
    assert(free_elem);

    heap_t new_heap = (heap_t) malloc(sizeof(struct heap));
    assert(new_heap);

    new_heap->elems = (void **) malloc(sizeof(void *) * DEFAULT_CAPACITY);
    assert(new_heap->elems);

    new_heap->capacity   = DEFAULT_CAPACITY;
    new_heap->size       = 0;
    new_heap->comparator = comparator;
    new_heap->free_elem  = free_elem;

    return new_heap;
};

/*  Free a heap - freeing a heap involves first calling the free_elem routine
    provided at creation (in the heap_create function) on all of the elements,
    then freeing the heap structure itself.*/
void heap_free(heap_t heap) {
    assert(heap);

    int i;
    for (i = 0; i < heap->size; i++) {
        heap->free_elem(heap->elems[i]);
    }

    free((void *) heap);
}

void heap_insert(heap_t, void *);
void *heap_min(heap_t);
void *heap_pop_min(heap_t);
unsigned int heap_size();

/*  Helper function implementations. */