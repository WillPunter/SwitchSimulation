/*  network_switch.c */
#include "network_switch.h"
#include <assert.h>
#include <stddef.h>

/*  Port mapping functions. */
struct addr {
    unsigned int addr;
};

struct port {
    unsigned int port;
};

typedef struct port_mapping_entry *port_mapping_t;

/*  Forward declare helper functions. */
static addr_t create_addr(unsigned int addr);
static void free_addr(addr_t addr);
static comparison_t addr_compare(void *lhs, void *rhs);
static port_t create_port(unsigned int port);
static void free_port(port_t port);
static hash_t addr_hash(void *addr_ptr);

/*  Network switch API implementation. */

/*  Network switch create - a network switch consists of n VOQs. A 2d buffer
    of VOQs must be allocated, and then each VOQ initialised. */
network_switch_t network_switch_create(
    unsigned int num_ports,
    free_func_t free_packet,
    schedule_func_t schedule,
    packet_read_addr_func_t read_addr
) {
    /*  Allocate memory for network switch structure. */
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    /*  Allocate buffer for VOQs and allocate their memory. */
    network_switch->num_ports = num_ports;
    network_switch->free_packet = free_packet;
    network_switch->schedule = schedule;
    network_switch->read_addr = read_addr;
    network_switch->voqs =
        (network_switch_t *) malloc(sizeof(queue_t) * num_ports * num_ports);
    assert(network_switch->voqs);

    int i;
    int j;
    for (i = 0; i < network_switch->num_ports; i++) {
        for (j = 0; j < network_switch->num_ports; j++) {
            network_switch->voqs[i][j] =
                queue_create(network_switch->free_packet);
            assert(network_switch->voqs[i][j]);
        };
    };

    /*  Create port mapping table. */
    network_switch->port_mapping = hash_table_create(
        addr_hash,
        addr_compare,
        free_addr, 
        free_port
    );

    return network_switch;
};

/*  Network switch receive packet - this simulates the receiving of a packet.
    The user must provide the switch with a hard-coded mapping of port numbers
    to addresses, stored in a hash table. */
void network_switch_recv_packet(
    network_switch_t network_switch,
    void *packet,
    port_t input_port) {

    addr_t addr = network_switch->read_addr(packet);

    /*  Lookup addr in table to find corresponding output port. */
    port_t output_port =
        (port_t) hash_table_lookup(network_switch->port_mapping, addr);
    
    if (output_port != NULL) {
        queue_enqueue(
            network_switch->voqs[input_port->port][output_port->port],
            packet
        );
    };
};

/*  Network switch free - free all packets stored in the VOQs, then free the
    buffer for the VOQs and finally the network switch structure itself. */
void network_switch_free(network_switch_t network_switch) {
    int i;
    int j;

    for (i = 0; i < network_switch->num_ports; i++) {
        for (j = 0; j < network_switch->num_ports; j++) {
            queue_free(network_switch->voqs[i][j]);
        };
    };

    free((void *) network_switch->voqs);

    hash_table_free(network_switch->port_mapping);

    free((void *) network_switch);
};

/*  Helper function implementations. */
static addr_t create_addr(unsigned int addr_value) {
    addr_t addr = malloc(sizeof(struct addr));
    assert(addr);

    addr->addr = addr_value;
    return addr;
};

static void free_addr(addr_t addr) {
    free((void *) addr);
};

static comparison_t addr_compare(void *lhs, void *rhs) {
    assert(lhs);
    assert(rhs);

    addr_t addr_lhs = (addr_t) lhs;
    addr_t addr_rhs = (addr_t) rhs;

    if (addr_lhs->addr < addr_rhs->addr) {
        return LT;
    } else if (addr_lhs->addr > addr_rhs->addr) {
        return GT;
    } else {
        return EQ;
    };
};

static port_t create_port(unsigned int port_value) {
    port_t port = malloc(sizeof(struct port));
    assert(port);

    port->port = port_value;
    return port;
};

static void free_port(port_t port) {
    free ((void *) port);
};

static hash_t addr_hash(void *addr_ptr) {
    assert(addr_ptr);

    addr_t addr = (addr_t) addr_ptr;
    
    return (hash_t) addr->addr;
};