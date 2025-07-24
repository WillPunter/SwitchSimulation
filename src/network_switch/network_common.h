/*  network_common.h

    This stores definitions relevant to all network swich code. */

#ifndef NETWORK_COMMON_H
#define NETWORK_COMMON_H

#define PAYLOAD_SIZE 64
#define PACKET_SIZE sizeof(struct packet)

struct addr {
    unsigned int value;
};

typedef struct addr addr_t;

struct packet_header {
    struct addr;
};

typedef struct packet_header packet_header_t;

struct packet {
    struct packet_header;
    char payload[PAYLOAD_SIZE];
};

typedef struct packet packet_t;

#endif