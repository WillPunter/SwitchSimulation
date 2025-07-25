/*  switch_cb_ib_voqs_iSLIP.h

    This is a crossbar (cb) network switch using input buffering (ib) with
    virtual output queues (voqs) usin the iSLIP scheduler. This is a switch
    architecture and implementation, so is to be combined with a network_switch
    structure from the network_switch.c file. */

#ifndef SWITCH_CB_IB_VOQS_ISLIP_H
#define SWITCH_CB_IB_VOQS_ISLIP_H

#include "./../switch_interfaces.h"

struct switch_cb_ib_voqs_iSLIP;

typedef struct switch_cb_ib_voqs_iSLIP *switch_cb_ib_voqs_iSLIP_t;

/*  The public API of this and any switch implementation is simply just a set
    of functions to construct each of the different kinds of interface. */
i_generic_switch_t generic_switch_cb_ib_voqs_iSLIP_create(
    port_num_t num_ports
);
i_cycle_simulation_switch_t cycle_simulation_switch_cb_ib_voqs_iSLIP_create(
    port_num_t num_ports
);

#endif