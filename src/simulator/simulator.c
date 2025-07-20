/*  simulator.c

    The simulator needs to maintain an event queue and a current time. Events
    can be invoked for the future.
    
    The event queue should be implemented as a heap comprised of event
    structures. These event structures should store an event ID by which to
    index them in an event table, which contains the corresponding callback
    function to be executed.

    The simulation then simply runs in a loop, where it dequeues the soonest
    event and runs its corresponding callback with its provided argument.
     
    The main source of complexity is handling different notions of time. */
#include "simulator.h"
#include "heap.h"

extern callback_func_t evt_table[MAX_EVENTS];

enum time_type {
    TIME_UINT,
    TIME_DOUBLE,
    TIME_CUSTOM
};

typedef enum time_type time_type_t;

struct event {
    unsigned int event_id;
    void *arg;
    void *time;
    evt_queue_t host_queue;
};

struct evt_queue {
    heap_t heap;
    comparator_func_t time_comparator;
    free_func_t time_free;
};

typedef struct evt_queue *evt_queue_t;

typedef struct event *event_t;

static comparison_t compare_event(void *lhs_ptr, void *rhs_ptr) {
    assert(lhs_ptr);
    assert(rhs_ptr);
    
    event_t lhs = (event_t) lhs_ptr;
    event_t rhs = (event_t) rhs_ptr;

    assert(lhs->host_queue == rhs->host_queue);

    return lhs->host_queue->time_comparator(lhs->time, rhs->time);
}

static void free_event(void *evt_ptr) {
    assert(evt_ptr);
    event_t evt = (event_t) evt_ptr;

    assert(evt->host_queue);
    evt->host_queue->time_free(evt->time);

    free(evt);
}

struct time_uint {
    unsigned int time;
};

typedef struct time_uint *time_uint_t;

struct time_double {
    double time;
};

typedef struct time_double *time_double_t;

static comparison_t compare_uint_time(void *lhs_ptr, void *rhs_ptr) {
    assert(lhs_ptr);
    assert(rhs_ptr);

    time_uint_t lhs = (time_uint_t) lhs_ptr;
    time_uint_t rhs = (time_uint_t) rhs_ptr;

    if (lhs->time < rhs->time) {
        return LT;
    } else if (lhs->time > rhs->time) {
        return GT;
    } else {
        return EQ;
    }
}

static time_uint_t time_uint_create(unsigned int value) {
    time_uint_t time = (time_uint_t) malloc(sizeof(struct time_uint));
    assert(time);

    time->time = value;
    return time;
}

static void free_uint_time(void *uint_ptr) {
    assert(uint_ptr);
    free(uint_ptr);
}

static comparison_t compare_double_time(void *lhs_ptr, void *rhs_ptr) {
    assert(lhs_ptr);
    assert(rhs_ptr);

    time_double_t lhs = (time_double_t) lhs_ptr;
    time_double_t rhs = (time_double_t) rhs_ptr;

    if (lhs->time < rhs->time) {
        return LT;
    } else if (lhs->time > rhs->time) {
        return GT;
    } else {
        return EQ;
    }
}

static time_double_t time_double_create(double value) {
    time_double_t time = (time_double_t) malloc(sizeof(struct time_double));
    assert(time);

    time->time = value;
    return time;
}

static void free_double_time(void *double_ptr) {
    assert(double_ptr);
    free(double_ptr);
}

/*  Create an event queue with the choice of unsigned integer or double default
    time implementations or a custom time implementation.
    
    If a custom time representation is chosen, then a routine for comparing
    said times and also for freeing the time structures must be provided.
    Otherwise, these may be NULL. */
evt_queue_t evt_queue_create(
    time_type_t time_type,
    comparator_func_t compare_time,
    free_func_t free_time
) {
    evt_queue_t queue = malloc(sizeof(struct evt_queue));
    assert(queue);

    /*  Set time related members, using uint and double defaults if set. */
    switch (time_type) {
        case TIME_UINT: {
            queue->time_comparator = compare_uint_time;
            queue->time_free = free_uint_time;
            break;
        }

        case TIME_DOUBLE: {
            queue->time_comparator = compare_double_time;
            queue->time_free = free_double_time;
            break;
        }

        case TIME_CUSTOM: {
            assert(compare_time);
            assert(free_time);
            queue->time_comparator = compare_time;
            queue->time_free = free_time;
            break;
        }
    }

    /*  Initialise heap. */
    queue->heap = heap_create(compare_event, free_event);

    return queue;
}