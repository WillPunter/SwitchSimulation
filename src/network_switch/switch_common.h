/*  switch_common.h

    Common switch-relevant definitions. */

#ifndef SWITCH_COMMON_H
#define SWITCH_COMMON_H

const int PACKET_SIZE = 64;
typedef void *packet_t;

typedef unsigned int port_num_t;

struct host_descriptor {
    void *addr;
    char active;
};

typedef struct host_descriptor host_descriptor_t;

#endif