/*  event_table.h

    The event table is a specific type of hash map which maps an integer index,
    representing an event id to a callback function which takes one generic
    argument, and a free function for said argument structure, which may
    optionally be NULL. */

#ifndef EVENT_TABLE_H
#define EVENT_TABLE_H

#include "heap.h"

struct event_table;
typedef struct event_table *event_table_t;

typedef int event_id_t;
typedef void (*callback_func_t)(void *);

/*  Event table API. */
event_table_t event_table_create();
void event_table_free(event_table_t event_table);
void event_table_register_event(
    event_table_t event_table,
    event_id_t evt_id,
    callback_func_t callback,
    free_func_t free_arg
);
void event_table_lookup(
    event_table_t event_table,
    event_id_t evt_id,
    callback_func_t *callback_out,
    free_func_t *free_arg_out
);

#endif