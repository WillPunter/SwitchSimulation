/*  network_switch.c

    Implementation of generic switch wrapper. */


/*  IDEA - maybe we can move the packet address to port mapping inside the
    generic switch and simply just have the internal switch logic deal with
    ports rather than addresses? */

#include "network_switch.h"
#include <assert.h>

/*  Port number structure used for hash table. */
struct port_num_value {
    port_num_t value;
};

typedef struct port_num_value *port_num_value_t;

/*  Forward declare helper functions. */
port_num_value_t port_num_value_create(port_num_t value);
void port_num_value_free(void *port_num_value);
port_num_t port_num_value_get(port_num_value_t port_num_value);

/*  Generic switch API implementation. */

/*  Initialise network switch structure - this involves */
void network_switch_init(
    network_switch_t *network_switch,
    port_num_t num_ports,
    void *switch_logic,
    hash_func_t addr_hash,
    comparator_func_t addr_compare,
    free_func_t addr_free
) {
    assert(network_switch);

    network_switch->num_ports = num_ports;

    network_switch->input_ports =
        (char (*)[PACKET_SIZE]) malloc(PACKET_SIZE * sizeof(char) * num_ports);
    assert(network_switch->input_ports);

    network_switch->input_port_occupied =
        (char *) malloc(sizeof(char) * num_ports);
    assert(network_switch->input_port_occupied);

    network_switch->output_ports =
        (char (*)[PACKET_SIZE]) malloc(
            PACKET_SIZE * sizeof(char) * PACKET_SIZE
        );
    assert(network_switch->output_ports);

    network_switch->output_port_occupied =
        (char *) malloc(sizeof(char) * num_ports);
    assert(network_switch->output_port_occupied);

    network_switch->hosts = (void **) malloc(sizeof(void *) * num_ports);
    assert(network_switch->hosts);

    network_switch->addr_table =
        hash_table_create(
            addr_hash,
            addr_compare,
            addr_free,
            port_num_value_free
        );
    assert(network_switch->addr_table);

    network_switch->switch_logic = switch_logic;
};

void network_switch_free(network_switch_t *network_switch);

void network_switch_register_host(
    network_switch_t *network_switch,
    void *host,
    port_num_t port_num
);

void network_switch_deregister_host(
    network_switch_t *network_switch,
    port_num_t port
);

void network_switch_recv_packet(
    network_switch_t *network_switch,
    void *packet,
    port_num_t input_port
);

void network_switch_send_packet(
    network_switch_t *network_switch,
    port_num_t output_port
);

/*  Helper function implementations. */
port_num_value_t port_num_value_create(port_num_t value) {
    port_num_value_t port_num_value
        = (port_num_value_t) malloc(sizeof(struct port_num_value));
    
    port_num_value->value = value;

    return port_num_value;
};

void port_num_value_free(void *port_num_value) {
    free(port_num_value);
};

port_num_t port_num_value_get(port_num_value_t port_num_value) {
    return port_num_value->value;
};