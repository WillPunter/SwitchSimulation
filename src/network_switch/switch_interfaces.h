/*  switch_interfaces.h

    This file defines a variety of interfaces for a generic switch to other
    hosts, and to different types of simulation, such as a cycle based
    simulation and a discrete event simulation.*/

#ifndef SWITCH_INTERFACES_H
#define SWITCH_INTERFACES_H

#include "switch_common.h"

/*  Argument functions for the interface functions. */

/*  Note that in the following structure, the prefix i stands for "interface".
    The i_func prefix refers to "interface function" to differentiate it from
    similarly named function types in other modules. */

/*  Initialisation function - arguments are number of ports and descriptor. */
typedef void *(*i_func_init_t)(port_num_t, void *);

/*  Free function - the argument is the structure to free. */
typedef void (*i_func_free_t)(void *);

/*  Register host - the arguments are the underlying structure, the host and
    the port to bind to. The host descriptor might need to, in some way,
    provide a function to actually get the data from the switch internal
    representation to the host representation as this is presumably host
    specific. */
typedef void (*i_func_register_host_t)(void *, void *, port_num_t);

/*  Deregister host - the arguments are the underlying structure and the port
    number to deregister. */
typedef void (*i_func_deregister_host_t)(void *, port_num_t);

/*  Receive packet - the arguments are the underlying structure, the packet
    data and the input port number to receive on. */
typedef void (*i_func_recv_packet_t)(void *, packet_t, port_num_t);

/*  Generic switch interface - this provides an interface for a generic switch.
    This provides routines for initialising and freeing the underlying
    structure, as well as configuring the port mappings (i.e. "plugging" a host
    in) and receiving packets on an input port. */
struct i_generic_switch {
    void *data;
    i_func_init_t init;
    i_func_free_t free;
    i_func_register_host_t register_host;
    i_func_deregister_host_t deregster_host;
    i_func_recv_packet_t recv_packet;
};

typedef struct generic_switch generic_switch_t;

/*  Cycle interface - this provides an interface for a switch to be used in a
    cycle based simulation. Since all operations are performed in one cycle,
    this simply must implement the basic setup functions (init, free, register,
    deregister, etc.) and also functions for sending traffic to the input ports
    and then ticking - i.e. computing one time slot / cycle operation. In the
    simulation, the traffic should be sent to an input port (for all input
    ports due to receive traffic in said cycle) and then after all traffic has
    been sent, the tick call must be made. */
struct i_cycle_simulation_switch {
    void *data;
    i_func_init_t init;
    i_func_free_t free;
    i_func_register_host_t register_host;
    i_func_deregister_host_t register_host;
    i_func_recv_packet_t recv_packet;
    void (*tick)();
};

/* Discrete event interface - TODO: figure out how this should work. */

#endif