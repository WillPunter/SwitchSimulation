/*  network_switch.c */
#include "network_switch.h"
#include <assert.h>

/*  Network switch create - a network switch consists of n VOQs.*/
network_switch_t network_switch_create(unsigned int num_ports) {
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    network_switch->num_ports = num_ports;
    network_switch->voqs =
        (network_switch_t *) malloc(sizeof(queue_t) * num_ports * num_ports);
    assert(network_switch->voqs);

    return network_switch;
};