/*  cycle_simulation.c

    This implements an incredibly simple mode of a switch, where we assume that
    all logic and packet processing takes place within a single time slot.
    We assume that in a real switch, these things would run concurrently so it
    makes sense for them to all take place within a time slot. In terms of
    translating a time slot into a real time unit - a single time slot is
    supposed to be the time taken for a packet to be outputted. This evaluates
    to:
        PACKET_SIZE / OUTPUT BANDWIDTH
    Where the output bandwidth of one port is also called the line rate. */

/*  Define duration of a time slot in terms of numbers of cycles - this allows
    flexibility even if it is unlikely to change from 1. */
#include "./network_switch/implementations/cb_ib_voqs_iSLIP.h"

#define TIME_SLOT 1
#define NUM_PORTS 8

/*  Packet format - */

/*  Create custom address format. */

int main(int argc, char *argv[]) {
    /*  Get switch interface implementaiton/ */
    i_cycle_sim_switch_t network_switch_desc = cb_ib_voqs_iSLIP_cycle_switch();

    void *network_switch = network_switch_desc.create(NUM_PORTS);

    /* Register hosts. */

    int running = 1;

    while (running) {
        network_switch_desc.tick();
    };

    network_switch_desc.free(network_switch);
};