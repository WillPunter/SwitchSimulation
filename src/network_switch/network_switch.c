/*  network_switch.c */
#include "network_switch.h"
#include <assert.h>

/*  Network switch create - a network switch consists of n VOQs. A 2d buffer
    of VOQs must be allocated, and then each VOQ initialised. */
network_switch_t network_switch_create(unsigned int num_ports, free_func_t free_packet) {
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    network_switch->num_ports = num_ports;
    network_switch->free_packet = free_packet;
    network_switch->voqs =
        (network_switch_t *) malloc(sizeof(queue_t) * num_ports * num_ports);
    assert(network_switch->voqs);

    int i;
    int j;
    for (i = 0; i < network_switch->num_ports; i++) {
        for (j = 0; j < network_switch->num_ports; j++) {
            network_switch->voqs[i][j] =
                queue_create(network_switch->free_packet);
            assert(network_switch->voqs[i][j]);
        };
    };

    return network_switch;
};

/*  Network switch free - free all packets stored in the VOQs, then free the
    buffer for the VOQs and finally the network switch structure itself. */
void network_switch_free(network_switch_t network_switch) {
    int i;
    int j;

    for (i = 0; i < network_switch->num_ports; i++) {
        for (j = 0; j < network_switch->num_ports; j++) {
            queue_free(network_switch->voqs[i][j]);
        };
    };

    free((void *) network_switch->voqs);
    free((void *) network_switch);
};