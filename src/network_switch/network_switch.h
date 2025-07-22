/*  switch.h

    This is the data model of the custom network switch. */

#ifndef SWITCH_H
#define SWITCH_H

#include "./../data_structures/queue.h"

typedef int *(*schedule_func_t)(void *);

struct network_switch {
    unsigned int num_ports;
    queue_t **voqs;
};

typedef struct network_switch *network_switch_t;

/*  Network switch API. */
network_switch_t network_switch_create(int num_ports);

#endif