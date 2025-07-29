/*  cycle_simulation.c */
#include "cycle_simulation.h"
#include <stddef.h>

static void *network_switch;
static i_cycle_sim_switch_t switch_interface;

/*  Initialise the cycle simulator - this involves setting the global switch to
    the provided cycle switch interface. */
enum cs_res cs_init(
    port_num_t num_ports,
    i_cycle_sim_switch_t cycle_interface,
    addr_desc_t addr_desc,
    host_desc_t port_host_map[],
    int num_mappings
) {
    /*  Check that interface contains implementations for all necessary
        functions and set global switch interface if so. */
    if (!(
        cycle_interface.create          &&
        cycle_interface.free            &&
        cycle_interface.register_host   &&
        cycle_interface.deregister_host &&
        cycle_interface.tick
    )) {
        return CS_ERROR_BROKEN_INTERFACE;
    };

    switch_interface = cycle_interface;

    /*  Create switch structure using interface creation routine. */
    network_switch = NULL; /*   Reset to NULL. */
    
    network_switch = switch_interface.create(
        num_ports,
        addr_desc
    );

    if (!network_switch) {
        return CS_ERROR_CREATION_FAILED;
    };

    /* Register any host <-> port mappings provided upfront. */
    int i;
    for (i = 0; i < num_mappings; i++) {
        if (port_host_map[i].active) {
            cycle_interface.register_host(
                network_switch,
                port_host_map[i],
                i
            );
        };
    };

    return CS_SUCCESS;
};

/*  Register a host. */
enum cs_res cs_reg_host(host_desc_t host, port_num_t port) {
    assert(network_switch);
    assert(switch_interface);
    assert(switch_interface.register_host);

    register_result_t res =
        switch_interface.register_host(network_switch, host, port);

    if (res != REG_SUCCESS) {
        return CS_ERROR_REGISTER_FAILED;
    };

    return CS_SUCCESS;
};

enum cs_res cs_start();
enum cs_res cs_free();