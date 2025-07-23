/*  crossbar_switch.c

    This is an implementation of a generic crossbar switch. It consists of
    three components: the input ports, the crossbar and the output ports
    respectively. This structure and the functions defined on it are intended
    to be generic enough to allow the implementation of different crossbar
    switches (e.g. input buffered, output buffered, different scheduling
    algorithms, etc.). */

#include "crossbar_switch.h"
#include <assert.h>

/*  Implementation of crossbar switch structure. */
struct crossbar_switch {
    port_num_t num_ports;
    crossbar_switch_input_t input;
    crossbar_switch_crossbar_t crossbar;
    crossbar_switch_output_t output;
    crossbar_switch_input_free_func_t input_free;
    crossbar_switch_crossbar_free_func_t crossbar_free;
    crossbar_switch_output_free_func_t output_free;
};

/*  Crossbar switch API implementation. */

/*  Create crossbar switch - creating a crossbar switch simply involves
    allocating a crossbar switch structure and using the provided creation
    functions to create an input, crossbar and output element. */
crossbar_switch_t crossbar_switch_create(
    port_num_t num_ports,
    crossbar_switch_input_create_func_t input_create,
    crossbar_switch_crossbar_create_func_t crossbar_create,
    crossbar_switch_output_create_func_t output_create,
    crossbar_switch_input_free_func_t input_free,
    crossbar_switch_crossbar_free_func_t crossbar_free,
    crossbar_switch_output_free_func_t output_free
) {
    crossbar_switch_t crossbar_switch =
        (crossbar_switch_t) malloc(sizeof(struct crossbar_switch));
    assert(crossbar_switch);

    crossbar_switch->num_ports = num_ports;
    crossbar_switch->input_free = input_free;
    crossbar_switch->crossbar_free = crossbar_free;
    crossbar_switch->output_free = output_free;

    crossbar_switch->input = input_create(crossbar_switch->num_ports);
    assert(crossbar_switch->input);

    crossbar_switch->crossbar = crossbar_create(crossbar_switch->num_ports);
    assert(crossbar_switch->crossbar);

    crossbar_switch->output = output_create(crossbar_switch->num_ports);
    assert(crossbar_switch->output);

    return crossbar_switch;
};

/*  Free crossbar switch - freeing a crossbar switch structure involves freeing
    the input, crossbar and output elements independently and then freeing the
    underlying structure. */
void crossbar_switch_free(crossbar_switch_t crossbar_switch) {
    crossbar_switch->input_free(crossbar_switch->input);
    crossbar_switch->crossbar_free(crossbar_switch->crossbar);
    crossbar_switch->output_free(crossbar_switch->output);
    free((void *) crossbar_switch);
};

/*  Receive packet for crossbar switch - receiving a packet for the crossbar
    switch simply just delegates to the input structure. */
void crossbar_switch_recv_packet(
    crossbar_switch_t crossbar_switch,
    port_num_t input_port,
    void *packet
);

void crossbar_switch_schedule(crossbar_switch_t crossbar_switch);

void crossbar_switch_output(
    crossbar_switch_t crossbar_switch,
    port_num_t output_port
);
