/*  heap.h

    Generic array-based heap. */

#ifndef HEAP_H

/*  Forward declare heap in this file but define it in .c file as heap
    implementation is hidden to users for modularity. */
struct heap;
typedef struct heap *heap_t;

/*  Comparison result outputted by comparator. A type alias is also provided
    for neatness. */
enum comparison {
    LT, /*  Less than. */
    GT, /*  Greater than. */
    EQ  /*  Equal to. */
};

typedef enum comparison comparison_t;

/*  Define heap comparator and free function types. The generic nature of the
    heap means that functions for comparing elements (to enforce prioriy) and
    freeing the memory of said custom elements must be provided. */
typedef comparison_t (*comparator_func)(void *, void *);

/*  Heap API. */
heap_t heap_init();

#endif