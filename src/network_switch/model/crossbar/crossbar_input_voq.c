/*  crossbar_input_voq.c

    An implementation of the input module for an input buffered crossbar switch
    using virtual output queues (VOQs) to prevent head of line blocking. */

#include "crossbar_components.h"
#include "crossbar_input_voq.h"
#include "./../../../data_structures/queue.h"
#include "./../../network_common.h"
#include <assert.h>

/*  Input module structure for an input buffered crossbar switch comprised of
    VOQs for eachport. */
struct crossbar_input_voq {
    port_num_t num_ports;
    queue_t *voqs;
};

struct packet_elem {
    packet_t packet;
};

typedef struct packet_elem *packet_elem_t;

/*  Forward declare helper functions. */
static packet_elem_t create_packet_elem(packet_t packet);
static void free_packet_elem(void *packet_elem_ptr);

/*  Generic input module interface implementation (i.e. functions from
    crossbar_components.h). */
void *crossbar_input_voq_create(
    crossbar_switch_t host_switch,
    port_num_t num_ports
) {
    crossbar_input_voq_t input_voq =
        (crossbar_input_voq_t) malloc(sizeof(struct crossbar_input_voq));
    assert(input_voq);
    
    input_voq->num_ports = num_ports;

    int i;
    for (i = 0; i < input_voq->num_ports; i++) {
        input_voq->voqs[i] = queue_create(free_packet_elem);
        assert(input_voq->voqs[i]);
    };
};

/*  Free an input voq - this involves freeing all of the queues that were
    created and finally freing the structure itself. */
void crossbar_input_voq_free(crossbar_switch_input_t input) {
    /*  Free all queues. */
    crossbar_input_voq_t input_voq = (crossbar_input_voq_t) input->data;

    int i;
    for (i = 0; i < input_voq->num_ports; i++) {
        queue_free(input_voq->voqs[i]);
    };

    free(input->data);
    free(input);
};

/*  Receive packet - this involves receiving a packet on an input port and
    buffering it accordingly. Specifically, the address from the packet must be
    retrieved and then looked up in the address-port mapping. It should then be
    stored in the voq of the output port for the provided input port. */
void crossbar_input_voq_recv(
    crossbar_switch_input_t input,
    port_num_t port_num,
    void *packet
) {
    crossbar_input_voq_t input_voq = (crossbar_input_voq_t) input->data;

    /*  Get destination address. */

    /*  Look up output port. */

    /*  Buffer packet. */
    
};

/*  Implement helper functions. */
static packet_elem_t create_packet_elem(packet_t packet) {
    packet_elem_t elem = (packet_elem_t) malloc(sizeof(struct packet_elem));
    assert(elem);

    elem->packet = packet;

    return elem;
};

static void free_packet_elem(void *packet_elem_ptr) {
    free(packet_elem_ptr);
};