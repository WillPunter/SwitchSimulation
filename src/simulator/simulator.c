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

static event_table_t event_table;
static heap_t event_queue;
static comparator_func_t compare_time;
static free_func_t free_time;
static void *current_time;

/*  Forward declare helper functions. */
static comparison_t compare_event(void *lhs, void *rhs);
static void free_event(void *event);
static uint_time_t create_uint_time(unsigned int time);
static comparison_t compare_uint_time(void *lhs, void *rhs);
static void free_uint_time(void *uint_time);
static double_time_t create_double_time(double time);
static comparison_t compare_double_time(void *lhs, void *rhs);
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
    time comparison and freeing functions. */
void simulator_init(
    time_type_t time_type,
    comparator_func_t custom_time_compare,
    comparator_func_t custom_time_free
) {
    event_table = event_table_create();

    switch (time_type) {
        case UINT_TIME: {
            compare_time = compare_uint_time;
            free_time = free_uint_time;
            break;
        };

        case DOUBLE_TIME: {
            compare_time = compare_double_time;
            free_time = free_double_time;
            break;
        };

        case CUSTOM_TIME: {
            compare_time = custom_time_compare;
            free_time = custom_time_free;
            break;
        };
    }

    event_queue = heap_create(compare_event, free_event);
};

/*  Destroy the simulator - this involves freeing the event table and event
    queue. */
void simulator_destroy() {
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

/*  Invoke event - this adds an event to the event queue at the given time.
    This involves providing a time. */

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

/*  Free double time - simply free the provided double time structure. */
static void free_double_time(void *double_time) {
    assert(double_time);
    free(double_time);
};