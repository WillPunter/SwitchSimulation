/*  crossbar_switch.h

    This module defines a model for a generic crossbar switch. A crossbar
    switch consists of an input mechanism at each port, a crossbar that matches
    each input to an output and an output mechanism at each port. To allow
    maximum modularity and customisability, these are implemented
    independently, so the type define here can be used to glue together any
    possible crossbar switch by defining different input, crossbar and output
    implementation for the same interfaces.
    
    Functions are provided to invoke each of the following parts:
        - Input - receive a packet on a particular port.
        - Crossbar - schedule a port mapping.
        - Output - output a packet to a supplied target. */

#ifndef CROSSBAR_SWITCH
#define CROSSBAR_SWITCH

struct crossbar_switch;
struct crossbar_switch_input;
struct crossbar_switch_crossbar;
struct crossbar_switch_output;

typedef struct crossbar_switch *crossbar_switch_t;

typedef struct crossbar_switch_input *crossbar_switch_input_t;
typedef struct crossbar_switch_crossbar *crossbar_switch_crossbar_t;
typedef struct crossbar_switch_output *crossbar_switch_output_t;

/*  Assumption: port number must be an enumerable type - any integer type or
    enum is completely fine. This is because port_num_t is used as an array
    index. Said array could be switched to a hash table if necessary but
    obviously an array has faster indexing so it would be good to keep it this
    way while we can. */
typedef int port_num_t;

/*  Crossbar switch API. */
crossbar_switch_t crossbar_switch_create(
    port_num_t num_ports,
    crossbar_switch_input_t input,
    crossbar_switch_crossbar_t crossbar,
    crossbar_switch_output_t output
);

void crossbar_switch_free(crossbar_switch_t crossbar_switch);

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

#endif