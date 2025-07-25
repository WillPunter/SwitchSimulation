/*  host_table.h

    A structure providing a hash table from addresses to port numbers and a
    lookup table (array) from addresses to ports. Hosts can be registered or
    deregistered. */

#ifndef HOST_TABLE_H
#define HOST_TABLE_H

#include "network_switch_common.h"

struct host_table;
typedef struct host_table *host_table_t;

/*  API functions. */
host_table_t host_table_create(port_num_t num_ports, addr_desc_t addr_desc);
void host_table_free(host_table_t host_table);
register_result_t host_table_register(
    host_table_t host_table,
    host_desc_t host_desc,
    port_num_t port
);
register_result_t host_table_deregister(
    host_table_t host_table,
    port_num_t port
);
int host_table_port_lookup(
    host_table_t host_table,
    void *addr,
    port_num_t *port_out
);
int host_table_host_lookup(
    host_table_t host_table,
    port_num_t port,
    host_desc_t *host_out
);

#endif