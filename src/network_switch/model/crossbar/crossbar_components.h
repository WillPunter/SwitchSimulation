/*  crossbar_components.h

    This function defines the interfaces of a crossbar switch. The reason this
    not instead put in crossbar_switch.h is because this information should
    ideally be hidden to the code that uses the crossbar switch, and visible
    only to the code that implements the crossbar switch and its components. */

#ifndef CROSSBAR_COMPONENTS_H
#define CROSSBAR_COMPONENTS_H

#include "crossbar_switch.h"

/*  Create functions. */
typedef void *
    (*crossbar_switch_input_create_func_t)(port_num_t);
typedef void *
    (*crossbar_switch_crossbar_create_func_t)(port_num_t);
typedef void *
    (*crossbar_switch_output_create_func_t)(port_num_t);

/*  Free functions. */
typedef void
    (*crossbar_switch_input_free_func_t)(crossbar_switch_input_t);
typedef void
    (*crossbar_switch_crossbar_free_func_t)(crossbar_switch_crossbar_t);
typedef void
    (*crossbar_switch_output_free_func_t)(crossbar_switch_output_t);

/*  Input operation functions. */
typedef void
    (*crossbar_switch_input_recv_func_t)(
        crossbar_switch_input_t,
        port_num_t,
        void*
    );

/*  Crossbar operation functions. */
typedef void
    (*crossbar_switch_crossbar_schedule_func_t)(
        crossbar_switch_crossbar_t,
        port_num_t *mapping
    );

/*  Output operation functions. */
typedef void
    (*crossbar_switch_output_send_packet_func_t)(
        crossbar_switch_output_t,
        port_num_t
    );

/*  Input, crossbar and output functions. */
struct crossbar_switch_input {
    void *data;
    crossbar_switch_input_create_func_t create_input;
    crossbar_switch_input_free_func_t free_input;
    crossbar_switch_input_recv_func_t recv_input;
    crossbar_switch_t host_switch;
};

struct crossbar_switch_crossbar {
    void *data;
    crossbar_switch_crossbar_create_func_t create_crossbar;
    crossbar_switch_crossbar_schedule_func_t schedule_crossbar;
    crossbar_switch_crossbar_free_func_t free_crossbar;
    crossbar_switch_t host_switch;
};

struct crossbar_switch_output {
    void *data;
    crossbar_switch_output_create_func_t create_output;
    crossbar_switch_output_send_packet_func_t send_packet_output;
    crossbar_switch_output_free_func_t free_output;
    crossbar_switch_t host_switch;
};

#endif