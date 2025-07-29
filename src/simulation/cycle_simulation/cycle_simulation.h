/*  cycle_simulation.h */
#ifndef CYCLE_SIMULATION_H
#define CYCLE_SIMULATION_H

#include "./../../network_switch/network_switch_interfaces.h"

/*  Cycle simulation result. */
enum cs_res {
    CS_SUCCESS,
    CS_ERROR_BROKEN_INTERFACE,
    CS_ERROR_CREATION_FAILED,
    CS_ERROR_REGISTER_FAILED
};

/*  API functions. */
enum cs_res cs_init(i_cycle_sim_switch_t cycle_switch);
enum cs_res cs_start();
enum cs_res cs_free();

#endif