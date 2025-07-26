/*  cb_ib_voqs_iSLIP.c

    Crossbar, input buffered, virtual output queues, iSLIP-scheduled switch
    implementation. */

#include "cb_ib_voqs_iSLIP.h"
#include "./../data_structures/queue.h"
#include "./../host_table.h"
#include <assert.h>
#include <stddef.h>

/*  Switch structure. */
struct network_switch {
    port_num_t num_ports;
    queue_t **voqs;
    host_table_t host_table;
};

typedef struct network_switch *network_switch_t;

/*  Cycle switch interface implementation. */
static void free_packet(void *packet) {
    free(packet);
};

/*  Create a new switch of the cb_ib_voqs_iSLIP variety. */
static void *cb_ib_voqs_iSLIP_create(port_num_t num_ports, addr_desc_t addr_desc) {
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    network_switch->num_ports = num_ports;

    network_switch->voqs =
        (queue_t **) malloc(sizeof(queue_t *) * network_switch->num_ports);
    
    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        network_switch->voqs[i] = queue_create(&free_packet);
    };

    network_switch->host_table =
        host_table_create(network_switch->num_ports, addr_desc);
    
    return (void *) network_switch;
};

/*  Free a switch of the cb_ib_voqs_iSLIP variety. */
static void cb_ib_voqs_iSLIP_free(void *network_switch_ptr) {
    assert(network_switch_ptr);
    
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        queue_free(network_switch->voqs[i]);
    };

    free(network_switch->voqs);

    host_table_free(network_switch->host_table);
};

/*  Register a host. */
register_result_t cb_ib_voqs_iSLIP_cycle_switch_register_host(
    void *network_switch_ptr,
    host_desc_t host_desc,
    port_num_t port
) {
    assert(network_switch_ptr);
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    assert(network_switch->host_table);
    return host_table_register(network_switch->host_table, host_desc, port);
};

/*  Deregister a host. */

/*  Tick. */

/*  API implementation. */
i_cycle_sim_switch_t cb_ib_voqs_iSLIP_cycle_switch() {
    i_cycle_sim_switch_t cycle_switch;
    cycle_switch.create = cb_ib_voqs_iSLIP_create;
    cycle_switch.free = cb_ib_voqs_iSLIP_free;
    cycle_switch.register_host = NULL;
    cycle_switch.deregister_host = NULL;
    cycle_switch.tick = NULL;

    return cycle_switch;
};