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

static event_table_t event_table;
static heap_t event_queue;
static comparator_func_t compare_time;
static free_func_t free_time;
static void *current_time;

/*  Forward declare helper functions. */
static comparison_t compare_event(void *lhs, void *rhs);
static void free_event(void *event);


/*  Simulator API implementation. */

/*  Initialise simulator - this involves creating the event table and the event
    queue. */
void simulator_init(time_type_t time_type) {
    event_table = event_table_create();

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