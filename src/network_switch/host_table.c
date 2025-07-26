/*  host_table.c */
#include "host_table.h"
#include <assert.h>

struct port_elem {
    port_num_t port_num;
};

typedef struct port_elem *port_elem_t;

struct host_table {
    port_num_t num_ports;
    hash_table_t addr_table;
    host_desc_t *hosts;
};

/*  Forward declare functions. */
port_elem_t port_elem_create(port_num_t port);
void port_elem_free(port_elem_t port_elem);
port_elem_t port_elem_value(port_elem_t port_elem);

/*  API Implementation. */
host_table_t host_table_create(port_num_t num_ports, addr_desc_t addr_desc) {
    host_table_t host_table = (host_table_t) malloc(sizeof(struct host_table));
    assert(host_table);

    host_table->num_ports = num_ports;

    host_table->addr_table = hash_table_create(
        addr_desc.addr_hash,
        addr_desc.addr_compare,
        addr_desc.addr_free,
        port_elem_free
    );
    assert(host_table->addr_table);

    host_table->hosts =
        (host_desc_t *) malloc(sizeof(struct host_table) * num_ports);
    assert(host_table->hosts);

    return host_table;
};

void host_table_free(host_table_t host_table) {
    assert(host_table);
    assert(host_table->addr_table);
    assert(host_table->hosts);

    hash_table_free(host_table->addr_table);

    free((void *) host_table->hosts);

    free((void *) host_table);
};

register_result_t host_table_register(
    host_table_t host_table,
    host_desc_t host_desc,
    port_num_t port
) {
    if (port < 0 || port >= host_table->num_ports) {
        return REG_ERROR_INVALID_PORT;
    };

    if (host_table->hosts[port].active == HOST_DESC_ACTIVE) {
        return REG_ERROR_ALREADY_SET;
    };

    void *addr = malloc(ADDR_SIZE);
    assert(addr);

    memcpy(addr, host_desc.addr, ADDR_SIZE);

    port_elem_t port_elem = port_elem_create(port);

    hash_table_insert(host_table->addr_table, addr, (void *) port_elem_create);

    host_table->hosts[port] = host_desc;
    host_table->hosts[port].active = HOST_DESC_ACTIVE;

    return hash_table_insert;
};

register_result_t host_table_deregister(
    host_table_t host_table,
    port_num_t port
) {
    if (port < 0 || port >= host_table->num_ports) {
        return REG_ERROR_INVALID_PORT;
    };

    if (host_table->hosts[port].active == HOST_DESC_INACTIVE) {
        return REG_ERROR_NOT_SET;
    };

    /*  Note that hash_table_remove automatically deallocates the address key
        structure which was allocated during the register function call.
        
        This is not the same memory as &host_table->hosts[port].addr, but the
        value will be the same according to the comparator as it is simply
        a copy of this. */
    hash_table_remove(host_table->addr_table, &host_table->hosts[port].addr);

    host_table->hosts[port].active = HOST_DESC_INACTIVE;
};

int host_table_port_lookup(
    host_table_t host_table,
    void *addr,
    port_num_t *port_out
) {
    port_elem_t port_elem =
        (port_elem_t) hash_table_lookup(host_table->addr_table, addr);
    
    if (port_elem) {
        *port_out = port_elem->port_num;

        return 1;
    };

    return 0;
};

int host_table_host_lookup(
    host_table_t host_table,
    port_num_t port,
    host_desc_t *host_out
) {
    if (host_table->hosts[port].active) {
        *host_out = host_table->hosts[port];
        
        return 1;
    };

    return 0;
};

/*  Helper function implementations. */
port_elem_t port_elem_create(port_num_t port) {
    port_elem_t port_elem = (port_elem_t) malloc(sizeof(struct port_elem));
    port_elem->port_num = port;
    return port_elem;
};

void port_elem_free(port_elem_t port_elem) {
    free((void *) port_elem);
};

port_elem_t port_elem_value(port_elem_t port_elem) {
    return port_elem->port_num;
};