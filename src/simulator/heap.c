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
static inline unsigned int parent_index(unsigned int index);
static inline unsigned int left_child_index(unsigned int index);
static inline unsigned int right_child_index(unsigned int index);
static char has_child(heap_t heap, unsigned int index);
static unsigned int largest_child_greater_than(
    heap_t heap,
    unsigned int index
);
static void swap_elem(
    heap_t heap,
    unsigned int index_1,
    unsigned int index_2
);

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
};

/*  Insert a new element into a heap - a heap must always take the form of a 
    complete binary tree (where all layers but the last are full, and the last
    layer has its elements fill in from the left side) so to maintain this
    structure, we insert the new element at the end of the array (reallocating
    if / as necessary) and then bubble the new element up until it finds a
    place which preserves the heap property. This works because if the new node
    is smaller than its parent, then it will also be smaller than all of the
    parents' children. Similarly, the parent is smaller than all of its
    childrens' children so swapping the two will not affect the heap
    property. */
void heap_insert(heap_t heap, void *elem) {
    assert(heap);
    assert(elem);
    assert(heap->elems);
    assert(heap->comparator);

    /*  Resize if capacity exceeded. */
    if (heap->size + 1 > heap->capacity) {
        heap->capacity = heap->capacity * 2;
        heap->elems = (void **) realloc(sizeof(void *) * heap->capacity);
    }

    heap->elems[heap->size] = elem;
    heap->size += 1;

    unsigned int index = heap->size - 1;
    unsigned int parent = parent_index(index);
    
    while (
        index != 0 &&
        heap->comparator(heap->elems[index], heap->elems[parent]) == LT
    ) {
        /*  Swap parent and child elements. */
        swap_elem(heap, index, parent);

        /*  Update indices. */
        index = parent;
        parent = get_parent_index(index);
    }
};

/*  Get minimum element - simply return the element at the start of the array,
    assuming of course the size is greater than zero. */
void *heap_min(heap_t heap) {
    assert(heap->size > 0);
    return heap->elems[0];
};

/*  Pop elements from heap - popping the minimum elements from a heap simply
    works by swapping the last element in the array into the place of the
    first and then bubbling it down, by swapping it with the larger child
    node until it is smaller than both children. This, of course, all assumes
    that the number of elements is greater than zero. */
void *heap_pop_min(heap_t heap) {
    assert(heap->size > 0);
    void *min = heap->elems[0];

    /*  Swap last and first elements and bubble down. */
    heap->elems[0] = heap->elems[heap->size - 1];
    heap->size -= 1;

    unsigned int index = 0;
    unsigned int child_index;

    while (
        has_child(heap, index) &&
        (child_index = largest_child_less_than(heap, index))
    ) {
        swap_elem(heap, index, child_index);
    }

    return min;
};

/*  Heap size - return heap size member. */
unsigned int heap_size(heap_t heap) {
    assert(heap);
    return heap->size;
};

/*  Helper function implementations. */

/*  Get parent index:
    Note that in the following we work with a 1 index for our tree and the
    underyling array.

    As a visual aid, the following is a binary tree with nodes labelled as
    indices.

        Array: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

        Complete binary tree:
        row | Tree
        1   |           1
            |          / \
            |         /   \
            |        /     \
        2   |       2       3
            |      / \     / \
            |     /   \   /   \
        3   |    4     5 6     7
            |   / \   /
        4   |  8   9 10

        Then the offset for each element on a given row is the position from
        the leftmost element on that row.

    Proof:
    
    We first assume / define the following relations based on the structure
    of a complete binary tree:
    
    row(x) = floor(log_2(x)) + 1
    offset(x) = x - 2^(row(x) - 1) + 1
              = x - 2^(floor(log_2(x)) + 1 - 1) + 1
              = x - 2^(floor(log_2(x))) + 1
    
    parent(x, y) = row(y) = row(x) + 1 and
                   (offset(y) = 2 * offset(x) - 1 or
                    offset(y) = 2 * offset(x))
    
    Some examples being:
        row(1) = floor(log_2(1)) + 1 = floor(0) + 1 = 1 
        
        row(6) = floor(log_2(6)) + 1 = floor(2.584962501) + 1 = 2 + 1 = 3

        offset(1) = 1 - 2^(floor(log_2(1))) + 1 = 1 - 2^(floor(0)) + 1 =
            1 - 2^0 + 1 = 1 - 1 + 1 = 0 + 1 = 1
        
        offset(6) = 6 - 2^(floor(log_2(6))) + 1 =
            6 - 2^(floor(2.584962501)) + 1 =
            6 - 2^(2) + 1 = 6 - 4 + 1 = 2 + 1 = 3
            
    Now we will prove that parent(x,y) -> y = 2x or y = 2x + 1.
    
    Assume parent(x, y). Then by the definition of the relation:
        (1) row(y) = row(x) + 1
        (2) offset(y) = 2 * offset(x) - 1 or
            offset(y) = 2 * offset(x)
    
    Expanding these based on their numerical definition:
        (1a) floor(log_2(y)) + 1 = floor(log_2(x)) + 1 + 1 [by def. row]
        (1b) floor(log_2(y)) = floor(log_2(x)) + 1         [by arithmetic]

        (2a) y - 2^(floor(log_2(y))) + 1 = 2(x - 2^(floor(log_2(x))) + 1) - 1
             or
             y - 2^(floor(log_2(y))) + 1 = 2(x - 2^(floor(log_2(x))) + 1)
             [by def. offset]

    Next, we substitute (1b) into (2a):
        (2b) y - 2^(floor(log_2(x)) + 1) + 1 =
                 2(x - 2^(floor(log_2(x))) + 1) - 1
            or
             y - 2^(floor(log_2(x)) + 1) + 1 =
                 2(x - 2^(floor(log_2(x))) + 1)
            [by 1b]
        
    And convert the 2^(f(x) + 1) to 2 * 2^(f(x)) by index laws.
        (2c) y - 2 * 2^(floor(log_2(x))) + 1 =
                 2(x - 2^(floor(log_2(x))) + 1) - 1
            or
             y - 2 * 2^(floor(log_2(x))) + 1 =
                 2(x - 2^(floor(log_2(x))) + 1)
            [by aritmetic]
    
    Then expand the brackets on the RHS.
        (2d) y - 2 * 2^(floor(log_2(x))) + 1 =
                 2x - 2 * 2^(floor(log_2(x))) + 2 - 1
            or
             y - 2 * 2^(floor(log_2(x))) + 1 =
                 2x - 2 * 2^(floor(log_2(x))) + 2
            [by aritmetic]
    
    Then add 2 * 2^(floor(log_2(x))) to both sides.
        (2e) y + 1 = 2x + 2 - 1 or
             y + 1 = 2x + 2
            [by aritmetic]
    
    Finally, simplifying gives:
        (2f) y = 2x     or
             y = 2x + 1
            [by aritmetic]

    as required.

    Q.E.D.

    Modifying this for 0 indexed values is easy - we just subtract one from
    the indices.

    Suppose in 1-index: x has children y = 2x or y = 2x + 1
    Then in 0-index: x - 1 has children y = 2x - 1 or y = 2x
    So if i = x - 1, then x = i + 1 and then i has children j = 2x - 1 or j = 2x
    so j = 2(i + 1) - 1 = 2i + 1 or j = 2(i + 1) = 2i + 2.
    So when 0-indexed, i has children j = 2i + 1 and 2i + 2. */
static inline unsigned int parent_index(unsigned int index) {
    return index / 2;
};

static inline unsigned int left_child_index(unsigned int index) {
    return 2 * index + 1;
}

static inline unsigned int right_child_index(unsigned int index) {
    return 2 * index + 2;
}

static char has_child(heap_t heap, unsigned int index) {
    return left_child_index(index) < heap->size ||
        right_child_index(index) < heap->size;
}

/*  Get the largest child node of the provided index for which the indexed node
    is greater than said child. If the indexed node is smaller than both of its
    children return 0, otherwise return the index of the largest child node
    that the provided node is greater than (for use in bubbling down during the
    heap pop function).*/
static unsigned int largest_child_greater_than(
    heap_t heap,
    unsigned int index
) {
    unsigned int left_child;
    unsigned int right_child;

    if ((right_child = right_child_index(index)) < heap->size) {
        /*  Node has two children by complete binary tree property of heap. */
        left_child = left_child_index(index);

        /*  Compare current and child elements. */
        void *curr = heap->elems[index];
        void *left = heap->elems[left_child];
        void *right = heap->elems[right_child];

        comparison_t curr_left = heap->comparator(curr, left);
        comparison_t curr_right = heap->comparator(curr, right);
        comparison_t left_right = heap->comparator(left, right);

        if (curr_left == GT && curr_right == GT) {
            /*  If curent > left and current > right then we want to return the
                larger of the left and right children. */
            return (left_right == GT) ? left_child : right_child; 
        } else if (curr_left == GT) {
            /* If current > left but not current > right then we only want to
                swap with the left. */
            return left_child;
        } else if(curr_right == GT) {
            /*  If current > right but not current > left then we only want to
                swap with the right. */
            return right_child;
        } else {
            /*  If current <= left and current <= right then current is in the
                right place and we do not want to move it. */
            return 0;
        }
    } else if ((left_child = left_child_index(index)) < heap->size) {
        /*  Node has only left child. */
        void *curr = heap->elems[index];
        void *left = heap->elems[left_child];

        comparison_t curr_left = heap->comparator(curr, left);
        
        if (curr_left == GT) {
            /*  If current > left then we swap current and left. */
            return left_child;
        } else {
            /*  If current <= left then current is in the right place. */
            return 0;
        }
    }

    /*  Node has no children. */

    return 0;
}

/*  Swap elements in heap with given indices. */
static void swap_elem(
    heap_t heap,
    unsigned int index_1,
    unsigned int index_2
) {
    void *temp = heap->elems[index_1];
    heap->elems[index_1] = heap->elems[index_2];
    heap->elems[index_2] = temp;
}