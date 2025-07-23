/*  crossbar_switch.c

    This is an implementation of a generic crossbar switch. It consists of
    three components: the input ports, the crossbar and the output ports
    respectively. This structure and the functions defined on it are intended
    to be generic enough to allow the implementation of different crossbar
    switches (e.g. input buffered, output buffered, different scheduling
    algorithms, etc.). */

#include "crossbar_switch.h"
#include "crossbar_components.h"
#include <assert.h>

/*  Implementation of crossbar switch structure. */
struct crossbar_switch {
    port_num_t num_ports;
    crossbar_switch_input_t input;
    crossbar_switch_crossbar_t crossbar;
    crossbar_switch_output_t output;
    port_num_t *input_output_map;
};

/*  Crossbar switch API implementation. */

/*  Create crossbar switch - creating a crossbar switch simply involves
    allocating a crossbar switch structure and using the provided creation
    functions to create an input, crossbar and output element. */
crossbar_switch_t crossbar_switch_create(
    port_num_t num_ports,
    crossbar_switch_input_t input,
    crossbar_switch_crossbar_t crossbar,
    crossbar_switch_output_t output
) {
    crossbar_switch_t crossbar_switch =
        (crossbar_switch_t) malloc(sizeof(struct crossbar_switch));
    assert(crossbar_switch);

    crossbar_switch->num_ports = num_ports;

    crossbar_switch->input_output_map =
        (port_num_t *) malloc(sizeof(port_num_t));
    assert(crossbar_switch->input_output_map);

    crossbar_switch->input = input;
    crossbar_switch->input->data =
        crossbar_switch->input->create_input(num_ports);
    assert(crossbar_switch->input->data);

    crossbar_switch->crossbar = crossbar;
    crossbar_switch->crossbar->data =
        crossbar_switch->crossbar->create_crossbar(num_ports);
    assert(crossbar_switch->crossbar->data);

    crossbar_switch->output = output;
    crossbar_switch->output->data =
        crossbar_switch->output->create_output(num_ports);
    assert(crossbar_switch->output->data);

    return crossbar_switch;
};

/*  Free crossbar switch - freeing a crossbar switch structure involves freeing
    the input, crossbar and output elements independently and then freeing the
    underlying structure. */
void crossbar_switch_free(crossbar_switch_t crossbar_switch) {
    crossbar_switch->input->free_input(crossbar_switch->input);
    crossbar_switch->crossbar->free_crossbar(crossbar_switch->crossbar);
    crossbar_switch->output->free_output(crossbar_switch->output);
    free((void *) crossbar_switch);
};

/*  Receive packet for crossbar switch - receiving a packet for the crossbar
    switch simply just delegates to the input structure. */
void crossbar_switch_recv_packet(
    crossbar_switch_t crossbar_switch,
    port_num_t input_port,
    void *packet
) {
    crossbar_switch->input->recv_input(
        crossbar_switch->input,
        input_port,
        packet
    );
};

/*  Schedule port-to-port mappings for crossbar switch - scheduling, i.e.
    computing the input-to-output port mappings is delegated to the scheduler
    and then used by the crossbar switch. */
void crossbar_switch_schedule(crossbar_switch_t crossbar_switch) {
    crossbar_switch->crossbar->schedule_crossbar(
        crossbar_switch->crossbar,
        crossbar_switch->input_output_map
    );
};

/*  Output packet on output port - this simply just delegates to the crossbar
    component. */
void crossbar_switch_output(
    crossbar_switch_t crossbar_switch,
    port_num_t output_port
) {
    crossbar_switch->output->send_packet_output(
        crossbar_switch->output,
        output_port
    );
};
