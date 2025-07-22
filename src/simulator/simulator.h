/*  simulator.h */

#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "heap.h"
#include "event_table.h"

typedef void (*add_func_t)(void *, void *);
typedef void (*copy_func_t)(void *, void *);

enum time_type {
    UINT_TIME,
    DOUBLE_TIME,
    CUSTOM_TIME
};

typedef enum time_type time_type_t;

/*  Simulator API. */
void simulator_init(
    time_type_t time_type,
    add_func_t custom_time_add,
    comparator_func_t custom_time_compare,
    copy_func_t custom_time_copy,
    free_func_t custom_time_free,
    void *start_time
);

void simulator_terminate();

void simulator_register_event(
    event_id_t evt_id,
    callback_func_t callback,
    free_func_t free_func
);

void simulator_invoke_event(event_id_t evt_id, void *arg, void *future_time);

void simulator_main_loop();

void simulator_set_should_terminate();

#endif