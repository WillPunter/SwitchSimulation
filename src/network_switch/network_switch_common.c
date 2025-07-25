/*  network_switch_common.c */

#include "network_switch_common.h"
#include <assert.h>
#include <string.h>

/*  API function implementations. */

/*  Create address descriptor structure. */
addr_desc_t addr_desc_create(
    void (*get_addr_from_packet)(void *),
    hash_func_t addr_hash,
    comparator_func_t addr_compare,
    free_func_t addr_free
) {
    addr_desc_t addr_desc = (addr_desc_t) malloc(sizeof(struct addr_desc));
    assert(addr_desc);

    addr_desc->get_addr_from_packet = get_addr_from_packet;
    addr_desc->addr_hash = addr_hash;
    addr_desc->addr_compare = addr_compare;
    addr_desc->addr_free = addr_free;

    return addr_desc;
};

/*  Free address descriptor structure. */
void addr_desc_free(addr_desc_t addr_desc) {
    free((void *) addr_desc);
};

/*  Create host descriptor structure. */
host_desc_t host_desc_create(
    void *data,
    void *addr,
    void (*send)(void *, void *),
    free_func_t addr_free
) {
    host_desc_t host_desc = (host_desc_t) malloc(sizeof(struct host_desc));
    assert(host_desc);

    /*  The underlying host structure is not cloned in memory, since once host
        may have many descriptors but each descriptor describes one host. */
    host_desc->data = data;

    /*  Each host descriptor structure should have its own address structure so
        that if one host descriptor has its address freed then other
        descriptors for the same underlying host can still be used. */
    host_desc->addr = addr_clone(addr);

    host_desc->send = send;
    host_desc->addr_free = addr_free;

    return host_desc;
};

/*  Free host descriptor structure. */
void host_desc_free(host_desc_t host_desc) {
    assert(host_desc);
    assert(host_desc->addr);

    /*  Free address structure. */
    free(host_desc->addr);

    /*  Free host descriptor structure. */
    free((void *) host_desc);
};

/*  Clone an address - addresses are used as data values in the hash table and
    are generic so must be represented dynamically. That being said, they are
    used like data values rather than like objects, which is why it is
    necessary to clone them. */
void *addr_clone(void *addr) {
    assert(addr);

    void *new_addr = malloc(ADDR_SIZE);
    assert(new_addr);

    memcpy(new_addr, addr, ADDR_SIZE);

    return addr;
};