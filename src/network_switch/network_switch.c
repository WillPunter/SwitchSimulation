/*  network_switch.c

    Implementation of generic switch wrapper. */

/*  IDEA - maybe we can move the packet address to port mapping inside the
    generic switch and simply just have the internal switch logic deal with
    ports rather than addresses? */

#include "network_switch.h"
#include <assert.h>
#include <string.h>

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

/*  Create a network switch structure - this creates and returns a new network
    switch wrapper structure. It initialises all of the required structures
    such as the intermediary packet buffers for the input and output ports and
    also the address to port lookup table. */
network_switch_t network_switch_create(
    port_num_t num_ports,
    void *switch_logic,
    func_send_packet_t switch_logic_send_packet,
    func_read_packet_dest_addr_t read_dest_addr,
    hash_func_t addr_hash,
    comparator_func_t addr_compare,
    free_func_t addr_free
) {
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
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

    network_switch->hosts =
        (host_descriptor_t *) malloc(
            sizeof(struct host_descriptor) * num_ports
        );
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
    network_switch->switch_logic_send_packet = switch_logic_send_packet;

    network_switch->read_dest_addr = read_dest_addr;

    return network_switch;
};

/*  Network switch free - this deallocates all of the structures allocated upon
    creation:
        - The address table.
        - The input and output packet buffers.
        - The input and output occupied flags.
        - The host buffer.
        - The underlying network switch structure. */
void network_switch_free(network_switch_t network_switch) {
    assert(network_switch);

    hash_table_free(network_switch->addr_table);

    free((void *) network_switch->input_ports);
    free((void *) network_switch->output_ports);

    free((void *) network_switch->input_port_occupied);
    free((void *) network_switch->output_port_occupied);

    free((void *) network_switch->hosts);

    free((void *) network_switch);
};

/*  Register host - this involves adding the host descriptor provided to
    the provided port number (if not already occupied) and then*/
register_outcome_t network_switch_register_host(
    network_switch_t network_switch,
    host_descriptor_t host_descriptor,
    port_num_t port_num
) {
    assert(network_switch);

    if (network_switch->hosts[port_num].active == 0) {
        network_switch->hosts[port_num] = host_descriptor;
        network_switch->hosts[port_num].active = 1;

        hash_table_insert(
            network_switch->addr_table,
            &network_switch->hosts[port_num].addr,
            (void *) port_num_value_create(port_num)
        );

        return REGISTER_SUCCESS;
    };

    return REGISTER_ALREADY_REGISTERED;
};

/*  Deregister host - this removes the host from the hosts buffer and the
    address table entry. */
register_outcome_t network_switch_deregister_host(
    network_switch_t network_switch,
    port_num_t port_num
) {
    assert(network_switch);

    if (network_switch->hosts[port_num].active == 1) {
        /*  NOTE: this may well cause a runtime error due to trying to free
            from an unallocated address - it might be necessary to refactor the
            hash table interface to prevent this. */
        hash_table_remove(
            network_switch->addr_table,
            &network_switch->hosts[port_num].addr
        );

        network_switch->hosts[port_num].active = 0;

        return REGISTER_SUCCESS;
    };

    return REGISTER_NOT_REGISTERED;
};

/*  Receive a packet - receiving a packet involves copying the packet into the
    corresponding buffer, reading it's header to obtain a destination address,
    identifying the output port it corresponds to and passing it to the switch
    logic. */
void network_switch_recv_packet(
    network_switch_t network_switch,
    void *packet,
    port_num_t input_port
) {
    assert(network_switch->input_port_occupied[input_port] == 0);

    memcpy(network_switch->input_ports[input_port], packet, PACKET_SIZE);
    network_switch->input_port_occupied[input_port] = 1;

    void *addr = network_switch->read_dest_addr(packet);
    void *output_port_ptr =
        hash_table_lookup(network_switch->addr_table, addr);
    assert(output_port_ptr);

    port_num_t output_port =
        port_num_value_get((port_num_value_t) output_port_ptr);
    
    network_switch->switch_logic_send_packet(
        network_switch,
        input_port,
        output_port
    );
};

/*  Network switch send packet - this simply copies the packet in the output
    buffer if present to the host using the provided send function. */
void network_switch_send_packet(
    network_switch_t network_switch,
    port_num_t output_port
) {
    assert(network_switch->output_port_occupied[output_port] == 1);
    assert(network_switch->hosts[output_port].active == 1);

    network_switch->hosts[output_port].send_packet(
        &network_switch->output_ports[output_port]
    );
};

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