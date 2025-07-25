/*  switch_cb_ib_voqs_iSLIP.c */

#include "switch_cb_ib_voqs_iSLIP.h"
#include "./../network_switch.h"
#include "./../../data_structures/queue.h"
#include <assert.h>

/*  Define the crossbar, input-buffered, virtual output queue and iSLIP
    scheduled switch structure. */
struct switch_cb_ib_voqs_iSLIP {
    queue_t ** voqs;
    port_num_t *input_output_map;
    network_switch_t parent_switch;
};

typedef struct switch_cb_ib_voqs_iSLIP *switch_logic_t;

/*  Forward declare helper functions. */
static switch_logic_t switch_logic_create(port_num_t num_ports);

static void recv_packet(
    void *network_switch,
    void *packet,
    port_num_t input_port,
    port_num_t output_port
);

/*  Public API implementaiton. */
i_generic_switch_t generic_switch_cb_ib_voqs_iSLIP_create(
    port_num_t num_ports,
    func_read_packet_dest_addr_t read_dest,
    hash_func_t addr_hash,
    comparator_func_t addr_compare,
    free_func_t addr_free
) {
    /*  Since the interface is just a set of pointers, we can just copy the
        struct without copying the data. */
    i_generic_switch_t generic_switch = {0};

    /*  Allocate new switch logic. */
    switch_logic_t switch_logic = (switch_logic_t) malloc(sizeof(switch_logic));
    assert(switch_logic);    

    /*  The switch logic must be combined with the generic network switch
        wrapper to provide a working switch. */
    network_switch_t parent_switch = network_switch_create(
        num_ports,
        (void *) switch_logic,
        recv_packet,
        read_dest,
        addr_hash,
        addr_compare,
        addr_free
    );
    assert(parent_switch);

    /*  Set properties of generic switch interface. */

    /*  Initialise data element. */

    return generic_switch;
};

i_cycle_simulation_switch_t cycle_simulation_switch_cb_ib_voqs_iSLIP_create(
    port_num_t num_ports
);

/*  Helper function implementations. */
static switch_logic_t switch_logic_create(port_num_t num_ports) {
    switch_logic_t switch_logic =
        (switch_logic_t) malloc(sizeof(struct switch_cb_ib_voqs_iSLIP));
    assert(switch_logic);

    switch_logic->voqs =
        (queue_t **) malloc(sizeof(queue_t) * num_ports * num_ports);
    assert(switch_logic->voqs);

    switch_logic->input_output_map =
        (port_num_t *) malloc(sizeof(port_num_t) * num_ports);
    assert(switch_logic->input_output_map);

    return switch_logic;
};