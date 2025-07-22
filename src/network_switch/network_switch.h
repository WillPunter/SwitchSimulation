/*  switch.h

    This is the data model of the custom network switch. */

#ifndef SWITCH_H
#define SWITCH_H

#include "./../data_structures/queue.h"
#include "./../data_structures/heap.h"
#include "./../data_structures/hash_table.h"

typedef int *(*schedule_func_t)(void *);

struct addr;
typedef struct addr *addr_t;

struct port;
typedef struct port *port_t;

typedef addr_t (*packet_read_addr_func_t)(void *);

struct network_switch {
    unsigned int num_ports;
    hash_table_t port_mapping;
    queue_t **voqs;
    free_func_t free_packet;
    schedule_func_t schedule;
    packet_read_addr_func_t read_addr;
};

typedef struct network_switch *network_switch_t;

/*  Network switch API. */
network_switch_t network_switch_create(int num_ports);

#endif