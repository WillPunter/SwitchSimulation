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
    void (*send_packet)(void *);
};

typedef struct host_descriptor host_descriptor_t;

typedef void (*func_send_packet_t)(void *, port_num_t, port_num_t);
typedef void *(*func_read_packet_dest_addr_t)(void *);

#endif