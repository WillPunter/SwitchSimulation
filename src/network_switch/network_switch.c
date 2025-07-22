/*  network_switch.c */
#include "network_switch.h"
#include <assert.h>

/*  Port mapping functions. */
struct addr {
    unsigned int addr;
};

typedef struct addr *addr_t;

struct port {
    unsigned int port;
};

typedef struct port *port_t;

typedef struct port_mapping_entry *port_mapping_t;

/*  Forward declare helper functions. */
static addr_t create_addr(unsigned int addr);
static port_t create_port(unsigned int port);
static hash_t addr_hash(void *addr_ptr);

/*  Network switch API implementation. */

/*  Network switch create - a network switch consists of n VOQs. A 2d buffer
    of VOQs must be allocated, and then each VOQ initialised. */
network_switch_t network_switch_create(unsigned int num_ports, free_func_t free_packet) {
    /*  Allocate memory for network switch structure. */
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    /*  Allocate buffer for VOQs and allocate their memory. */
    network_switch->num_ports = num_ports;
    network_switch->free_packet = free_packet;
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
    network_switch->port_mapping = hash_table_create();

    return network_switch;
};

/*  Network switch receive packet - this simulates the receiving of a packet.
    The user must provide the switch with a hard-coded mapping of port numbers
    to addresses, stored in a hash table. */
void network_switch_recv_packet(
    network_switch_t network_switch,
    void *packet) {
    /*  TODO. */
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
    free((void *) network_switch);
};

/*  Helper function implementations. */
static addr_t create_addr(unsigned int addr_value) {
    addr_t addr = malloc(sizeof(struct addr));
    assert(addr);

    addr->addr = addr_value;
    return addr;
};

static port_t create_port(unsigned int port_value) {
    port_t port = malloc(sizeof(struct port));
    assert(port);

    port->port = port_value;
    return port;
};

static hash_t addr_hash(void *addr_ptr) {
    assert(addr_ptr);

    addr_t addr = (addr_t) addr_ptr;
    
    return (hash_t) addr->addr;
};