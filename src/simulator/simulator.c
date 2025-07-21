/*  simulator.c */

#include "simulator.h"
#include "heap.h"
#include "event_table.h"

/*  Structures and types. */
struct event {
    event_id_t evt_id;
    void *arg;
    void *time;
};

typedef struct event *event_t;

enum time_type {
    UINT_TIME,
    DOUBLE_TIME,
    CUSTOM_TIME
};

typedef enum time_type time_type_t;

struct uint_time {
    unsigned int time;
};

typedef struct uint_time *uint_time_t;

struct double_time {
    double time;
};

typedef struct double_time *double_time_t;

typedef void (*add_func_t)(void *, void *);
typedef void (*copy_func_t)(void *, void *);

static event_table_t event_table;
static heap_t event_queue;
static add_func_t add_time;
static comparator_func_t compare_time;
static copy_func_t copy_time;
static free_func_t free_time;
static void *current_time;
static char should_terminate;

/*  Forward declare helper functions. */
static comparison_t compare_event(void *lhs, void *rhs);
static void free_event(void *event);
static uint_time_t create_uint_time(unsigned int time);
static void add_uint_time(void *lhs, void *rhs);
static comparison_t compare_uint_time(void *lhs, void *rhs);
static void copy_uint_time(void *lhs, void *rhs);
static void free_uint_time(void *uint_time);
static double_time_t create_double_time(double time);
static void add_double_time(void *lhs, void *rhs);
static comparison_t compare_double_time(void *lhs, void *rhs);
static void copy_double_time(void *lhs, void *rhs);
static void free_double_time(void *double_time);

/*  Simulator API implementation. */

/*  Initialise simulator - this involves creating the event table and the event
    queue. To provide flexibility, a time type parameter is passed. At time of
    writing, this provides two default time representations as unsigned
    integers and doubles but also allows a custom time representation to be
    used. If CUSTOM_TIME is passed for time_type, then custom_time_compare and
    custom_time_free must be non-NULL function pointers for valid comparison
    and memory freeing functions on the structure. If time_type is UINT_TIME or
    DOUBLE_TIME, i.e. one of the defaults, then NULL should be passed for the
    time comparison and freeing functions. The start time parameter should also
    only be provided with a non-NULL value when CUSTOM_TIME is passed, and
    should be the representation of the start time / zero time in the custom
    representation. */
void simulator_init(
    time_type_t time_type,
    add_func_t custom_time_add,
    comparator_func_t custom_time_compare,
    copy_func_t custom_time_copy,
    free_func_t custom_time_free,
    void *start_time
) {
    should_terminate = 0;

    event_table = event_table_create();

    switch (time_type) {
        case UINT_TIME: {
            add_time = add_uint_time;
            compare_time = compare_uint_time;
            copy_time = copy_uint_time;
            free_time = free_uint_time;
            current_time = create_uint_time(0);
            break;
        };

        case DOUBLE_TIME: {
            add_time = add_double_time;
            compare_time = compare_double_time;
            copy_time = copy_double_time;
            free_time = free_double_time;
            current_time = create_double_time(0.0);
            break;
        };

        case CUSTOM_TIME: {
            assert(custom_time_compare);
            assert(custom_time_free);
            assert(start_time);
            add_time = custom_time_add;
            compare_time = custom_time_compare;
            copy_time = custom_time_copy;
            free_time = custom_time_free;
            current_time = start_time;
            break;
        };
    }

    event_queue = heap_create(compare_event, free_event);
};

/*  Terminate the simulator - this involves freeing the event table and event
    queue. */
void simulator_terminate() {
    event_table_free(event_table);

    heap_free(event_queue);
};

/*  Register simulator event - this is simply a wrapper around the
    corresponding event table function. This is because the simulator module is
    the primary way that the user interfaces with the simulator and therefore
    the event table is encapsulated. */
void simulator_register_event(
    event_id_t evt_id,
    callback_func_t callback,
    free_func_t free_func
) {
    assert(event_table);
    event_table_register_event(event_table, evt_id, callback, free_func);
};

/*  Invoke event - this adds an event to the event queue at the given time in
    the future. The event id specifies the event, and provides a key for lookup
    in the event table. The arg parameter is the argument to be passed in. It
    may be NULL. The future_time parameter specifies how far in the future the
    event should occur, so that the event occurs at current_time +
    future_time.
    
    An event structure is allocated to store the event invocation properties.
    The event structure is deallocated when the event is dequeued or when the
    simulation is terminated. */
void simulator_invoke_event(event_id_t evt_id, void *arg, void *future_time) {
    event_t event = (event_t) malloc(sizeof(struct event));
    event->evt_id = evt_id;
    event->arg = arg;
    event->time = future_time;
    add_time(current_time, event->time);

    heap_insert(event_queue, (void *) event);
};

/*  Simulator main loop. */
void simulator_main_loop() {
    while (!should_terminate && heap_size(event_queue) > 0) {
        /*  Dequeue next event and advance current time. */
        void *next_event_ptr = heap_pop_min(event_queue);
        assert(next_event_ptr);

        event_t next_event = (event_t) next_event_ptr;
        time_copy(current_time, next_event->time);

        /* Lookup event in event table and execute callback with arg. */
        callback_func_t callback;
        free_func_t free_arg;
        
        event_table_lookup(
            event_table,
            next_event->evt_id,
            &callback,
            &free_arg
        );

        if (callback && free_arg) {
            callback(next_event->arg);
            free_arg(next_event->arg);
        };

        /*  Free time and event structures now dequeued. */
        free_time(next_event->time);
        free(next_event_ptr);
    };
};

/*  Helper function implementations. */

/*  Compare events - to compare two events, we simply invoke the local
    comparator function on their time values. */
static comparison_t compare_event(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);
    assert(compare_time);

    event_t evt_lhs = (event_t) lhs;
    event_t evt_rhs = (event_t) rhs;

    return time_comparator(evt_lhs->time, evt_rhs->time);
};

/*  Free event - to free an event we first free its argument and time values
    and then free the event structure. */
static void free_event(void *evt_ptr) {
    assert(evt_ptr);
    assert(free_time);

    event_t evt = (event_t) evt_ptr;

    callback_func_t lookup_callback;
    free_func_t lookup_free_arg;

    event_table_lookup(
        event_table,
        evt->evt_id,
        &lookup_callback,
        &lookup_free_arg
    );

    if (lookup_free_arg) {
        lookup_free_arg(evt->arg);
    };

    free_time(evt->time);

    free(evt_ptr);
};

/*  Create uint time - simply creates a uint_time structure with the provided
    time value. */
static uint_time_t create_uint_time(unsigned int time) {
    uint_time_t uint_time = (uint_time_t) malloc(sizeof(struct uint_time));
    uint_time->time = time;
    return uint_time;
};

/*  Add uint time - this modifies the value of the rhs time to contain the
    summation of the times of the lhs and rhs parameters. */
static void add_uint_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    uint_time_t time_lhs = (uint_time_t) lhs;
    uint_time_t time_rhs = (uint_time_t) rhs;

    time_rhs->time += time_lhs->time;
};

/*  Compare uint time - this simply compares the unsigned integer
    representations for simulation time. */
static comparison_t compare_uint_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    uint_time_t time_lhs = (uint_time_t) lhs;
    uint_time_t time_rhs = (uint_time_t) rhs;

    if (time_lhs->time < time_rhs->time) {
        return LT;
    } else if (time_lhs->time > time_rhs->time) {
        return GT;
    } else {
        return EQ;
    };
};

/*  Copy unit time - copy time value on lhs into rhs. */
static void copy_uint_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    uint_time_t time_lhs = (uint_time_t) lhs;
    uint_time_t time_rhs = (uint_time_t) rhs;

    time_rhs->time = time_lhs->time;
};

/*  Free uint time - simply frees the provided uint_time structure. */
static void free_uint_time(void *uint_time) {
    assert(uint_time);

    free(uint_time);
};

/*  Create double time - allocates and returns a double_time structure with the
    provided time value. */
static double_time_t create_double_time(double time) {
    double_time_t double_time =
        (double_time_t) malloc(sizeof(struct double_time));
    
    double_time->time = time;

    return double_time;
};

/*  Add double time - add the values of the provided times and store the result
    in the right hand side element. */
static void add_double_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    double_time_t time_lhs = (double_time_t) lhs;
    double_time_t time_rhs = (double_time_t) rhs;

    time_rhs->time += time_lhs->time;
};

/*  Compare double time - simply compares two double time structures by
    comparing their internal time elements with the <, >, == operators. */
static comparison_t compare_double_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    double_time_t time_lhs = (double_time_t) lhs;
    double_time_t time_rhs = (double_time_t) rhs;

    if (time_lhs->time < time_rhs->time) {
        return LT;
    } else if (time_lhs->time > time_rhs->time) {
        return GT;
    } else {
        return EQ;
    };
};

/*  Copy double time - copy the lhs time value into the rhs. */
static void copy_double_time(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    double_time_t time_lhs = (double_time_t) lhs;
    double_time_t time_rhs = (double_time_t) rhs;

    time_rhs->time = time_lhs->time;
}

/*  Free double time - simply free the provided double time structure. */
static void free_double_time(void *double_time) {
    assert(double_time);
    free(double_time);
};