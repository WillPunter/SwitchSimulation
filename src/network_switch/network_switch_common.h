/*  network_switch_common.h

    Definitions for common structures and types used in network switch and
    network switch adjacent modules. */

#ifndef NETWORK_SWITCH_COMMON_H
#define NETWORK_SWITCH_COMMON_H

#include "./../data_structures/heap.h"
#include "./../data_structures/hash_table.h"

/*  Address descriptor - this provides a set of pointers to functions required
    to obtain and process addresses. */
struct addr_desc {
    void (*get_addr_from_packet)(void *);
    hash_func_t addr_hash;
    comparator_func_t addr_compare;
    free_func_t addr_free;
};

typedef struct addr_desc addr_desc_t;

/*  Host descriptor - this stores data required to identify and interact with
    a host. */
struct host_desc {
    void *data;
    void *addr;
    void (*send)(void *, void *);
};

typedef struct host_desc host_desc_t;

/*  Port number - must be an integer type as used for array indexing. */
typedef unsigned int port_num_t;

/*  Register result - returned upon registering or deregistering a host. */
enum register_result {
    REG_SUCCESS,
    REG_ERROR_ALREADY_SET,
    REG_ERROR_NOT_SET
};

typedef enum register_result register_result_t;

#endif