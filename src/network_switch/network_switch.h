/*  network_switch.h

    This module implements the switch functionality present in all switches.
    This of course includes registering hosts and building the address -> port
    table, as well as receiving input. This module does not handle the actual
    switching process, but instead maintains a switch logic structure which
    provides the actual switch architecture and implementation - this file
    simply provides the architecture agnostic functions required of any
    switch.
    
    Note that the network_switch structure is not something that should be
    instantiated by itself - it must be combined with a switch logic structure.
    Therefore, this can be thought of as a generic interface which should not
    be created directly apart from by concrete switch implementations that
    combine it with some internal switch logic. This is why the provided
    API is so transparent. */

#ifndef NETWORK_SWITCH
#define NETWORK_SWITCH

#include "switch_common.h"
#include "./../data_structures/hash_table.h"

/*  Network switch wrapper function. */
struct network_switch {
    port_num_t num_ports;

    /*  Pointer to array of PACKET_SIZE chars - to be allocated to n buffers
        of PACKET_SIZE chars each for n ports. We need one of these for the
        input ports and output ports - note that these do NOT model input
        buffering, as that is done in the switch logic, these are simply used
        to provide a consistent interface to read / write for the switch logic
        as an intermediary between the host and the actual switch logic that
        is entirely virtual. */
    char (*input_ports)[PACKET_SIZE];
    char *input_port_occupied;
    char (*output_ports)[PACKET_SIZE];
    char *output_port_occupied;

    void **hosts;

    hash_table_t addr_table;
    void *switch_logic;
};

typedef struct network_switch *network_switch_t;

/*  Network switch API to switch logic (not to environment / simulation). */
network_switch_t network_switch_init(
    port_num_t num_ports,
    void *switch_logic,
    hash_func_t addr_hash,
    comparator_func_t addr_compare,
    free_func_t addr_free
);
void network_switch_free(network_switch_t network_switch);
void network_switch_register_host(
    network_switch_t network_switch,
    void *host,
    port_num_t port_num
);
void network_switch_deregister_host(
    network_switch_t network_switch,
    port_num_t port
);
void network_switch_recv_packet(
    network_switch_t network_switch,
    void *packet,
    port_num_t input_port
);
void network_switch_send_packet(
    network_switch_t network_switch,
    port_num_t output_port
);

#endif