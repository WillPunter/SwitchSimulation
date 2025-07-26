/*  cb_ib_voqs_iSLIP.c

    Crossbar, input buffered, virtual output queues, iSLIP-scheduled switch
    implementation. */

#include "cb_ib_voqs_iSLIP.h"
#include "./../data_structures/queue.h"
#include "./../host_table.h"
#include "./../network_switch_common.h"
#include <assert.h>
#include <stddef.h>
#include <math.h>

/*  Switch structure. */
struct network_switch {
    port_num_t num_ports;
    queue_t **voqs;
    host_table_t host_table;
    addr_desc_t addr_desc;

    port_num_t *iSLIP_grant_ptr;
    port_num_t *iSLIP_accept_ptr;

    port_num_t *port_match;
    char *port_match_active;
};

typedef struct network_switch *network_switch_t;

/*  Cycle switch interface implementation. */
static void free_packet(void *packet) {
    free(packet);
};

/*  Create a new switch of the cb_ib_voqs_iSLIP variety. */
static void *cb_ib_voqs_iSLIP_create(port_num_t num_ports, addr_desc_t addr_desc) {
    network_switch_t network_switch =
        (network_switch_t) malloc(sizeof(struct network_switch));
    assert(network_switch);

    network_switch->num_ports = num_ports;
    network_switch->addr_desc = addr_desc;

    network_switch->voqs =
        (queue_t **) malloc(sizeof(queue_t *) * network_switch->num_ports);
    assert(network_switch->voqs);
    
    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        network_switch->voqs[i] = queue_create(&free_packet);
        assert(network_switch->voqs[i]);
    };

    network_switch->host_table =
        host_table_create(network_switch->num_ports, addr_desc);
    assert(network_switch->host_table);
    
    network_switch->iSLIP_grant_ptr =
        (port_num_t *) malloc(sizeof(port_num_t) * network_switch->num_ports);
    assert(network_switch->iSLIP_grant_ptr);
    memset(
        network_switch->iSLIP_grant_ptr,
        0,
        sizeof(port_num_t) * network_switch->num_ports
    );
    
    network_switch->iSLIP_accept_ptr =
        (port_num_t *) malloc(sizeof(port_num_t) * network_switch->num_ports);
    assert(network_switch->iSLIP_accept_ptr);
    memset(
        network_switch->iSLIP_accept_ptr,
        0,
        sizeof(port_num_t) * network_switch->num_ports
    );
    
    network_switch->port_match =
        (port_num_t *) malloc(sizeof(port_num_t) * network_switch->num_ports);
    assert(network_switch->port_match);

    network_switch->port_match_active =
        (char *) malloc(sizeof(char) * network_switch->num_ports);
    assert(network_switch->port_match_active);

    return (void *) network_switch;
};

/*  Free a switch of the cb_ib_voqs_iSLIP variety. */
static void cb_ib_voqs_iSLIP_free(void *network_switch_ptr) {
    assert(network_switch_ptr);
    
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        queue_free(network_switch->voqs[i]);
    };

    free(network_switch->voqs);

    free(network_switch->iSLIP_grant_ptr);

    free(network_switch->iSLIP_accept_ptr);

    free(network_switch->port_match);

    free(network_switch->port_match_active);

    host_table_free(network_switch->host_table);
};

/*  Register a host. */
register_result_t cb_ib_voqs_iSLIP_cycle_switch_register_host(
    void *network_switch_ptr,
    host_desc_t host_desc,
    port_num_t port
) {
    assert(network_switch_ptr);
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    assert(network_switch->host_table);
    return host_table_register(network_switch->host_table, host_desc, port);
};

/*  Deregister a host. */
register_result_t cb_ib_voqs_iSLIP_cycle_switch_deregister_host(
    void *network_switch_ptr,
    port_num_t port_num
) {
    assert(network_switch_ptr);
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    assert(network_switch->host_table);
    return host_table_deregister(network_switch->host_table, port_num);
};

/*  iSLIP works as follows:
        Input round:
            The input ports send messages to all of the output ports they wish
            to output to.
            
        Output round:
            The output ports receive their messages and then choose one to
            grant starting from the output pointer entry.
        
        Input grant:
            The input ports then receive their grants and accept those starting
            from the grant.*/
static void iSLIP_schedule(
    network_switch_t network_switch,
    port_num_t *port_mapping,
    char *active_mapping
) {
    assert(network_switch);
    assert(port_mapping);
    assert(active_mapping);
    
    int r;
    int i;
    int j;

    /*  Reset schedule. */
    for (i = 0; i < network_switch->num_ports; i++) {
        network_switch->port_match[i] = 0;
        network_switch->port_match_active[i] = 0;
    };

    /*  Create arrays for matched inputs, matched outputs, request granted*/
    /*  input_matched[i] = 0 if input i unmatched, 1 if input i matched. */
    char *input_matched =
        (char *) malloc(sizeof(char) * network_switch->num_ports);
    assert(input_matched);
    memset(input_matched, 0, sizeof(char) * network_switch->num_ports);

    /*  output_matched[i] = 0 if output i unmatched, 1 if output i matched. */
    char *output_matched =
        (char *) malloc(sizeof(char) * network_switch->num_ports);
    assert(output_matched);
    memset(output_matched, 0, sizeof(char) * network_switch->num_ports);

    /*  grant_target[i] = j if output i has made grant to input j. Mapping only
        valid if grant_made[i] = 1 (see next). */
    port_num_t *grant_target =
        (port_num_t *) malloc(sizeof(port_num_t) * network_switch->num_ports);
    
    /*  grant_made[i] = 1 if output i has made a grant this round and
        grant_made[i] = 0 if output i has not made a grant this round. */
    char *grant_made =
        (char *) malloc(sizeof(char) * network_switch->num_ports);
    memset(grant_made, 0, sizeof(char) * network_switch->num_ports);

    /*  Iterate through rounds. */
    int grant_input_found;
    int match_found;

    for (r = 0; r < ISLIP_ROUNDS(network_switch->num_ports); r++) {
        /*  Reset grants. */
        memset(grant_made, 0, sizeof(char) * network_switch->num_ports);

        /*  Grant phase - each unmatched output cycles through unmatched inputs
            starting at the input_ptr and sends the grant to the first
            one with a packet to be sent to said output. */
        for (i = 0; i < network_switch->num_ports; i++) {
            if (output_matched[i] == 0) {
                grant_input_found = 0;

                for (j = 0; j < network_switch->num_ports && !grant_input_found; j++) {
                    port_num_t input_port =
                        (network_switch->iSLIP_grant_ptr[i] + j) %
                        network_switch->num_ports;

                    if (input_matched[input_port] == 0) {
                        if (queue_size(
                            network_switch->voqs[input_port][i]
                        ) > 0) {
                            grant_target[i] = input_port;
                            grant_made[i] = 1;
                            grant_input_found = 1;
                        };
                    };
                };
            };
        };

        /*  Accept phase - each unmatched input cycles through its grants and
            chooses the first one starting from output_ptr. It accepts the
            grant and the input and output become matched. */
        for (i = 0; i < network_switch->num_ports; i++) {
            if (input_matched[i] == 0) {
                match_found = 0;
                for (j = 0; j < network_switch->num_ports && !match_found; j++) {
                    port_num_t output_port =
                        (network_switch->iSLIP_accept_ptr[i] + j) %
                        network_switch->num_ports;
                    
                    if (output_matched[output_port] == 0) {
                        if (
                            grant_made[output_port] == 1 &&
                            grant_target[output_port] == i
                        ) {
                            /*  Accept. */
                            match_found = 1;
                            input_matched[i] = 1;
                            output_matched[output_port] = 1;
                            network_switch->port_match[i] = output_port;
                            network_switch->port_match_active[i] = 1;
                            network_switch->iSLIP_grant_ptr[output_port] =
                                (network_switch->iSLIP_grant_ptr[output_port] + 1) %
                                network_switch->num_ports;
                            network_switch->iSLIP_accept_ptr[i] =
                                (network_switch->iSLIP_accept_ptr[i] + 1) %
                                network_switch->num_ports;
                        };
                    };
                };
            };
        };
    };

    free(input_matched);
    free(output_matched);
    free(grant_target);
    free(grant_made);
};

/*  Tick - in a single tick of the cycle simulation, the switch should take in
    all received packets and buffer them, then should run the iSLIP scheduler
    and finally should output to the corresponding hosts. */
void cb_ib_voqs_iSLIP_cycle_switch_tick(
    void *network_switch_ptr,
    void *traffic_ptr
) {
    assert(network_switch_ptr);
    network_switch_t network_switch = (network_switch_t) network_switch_ptr;

    assert(traffic_ptr);
    void ** traffic = (void **) traffic_ptr;

    /*  Buffer incoming traffic. */
    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        if (traffic[i] != NULL) {
            void *addr =
                network_switch->addr_desc.get_addr_from_packet(traffic[i]);
            
            port_num_t output_port;
            int res = host_table_port_lookup(
                network_switch->host_table,
                addr,
                &output_port
            );

            if (res) {
                /*  Copy packet and buffer into corresponding VOQ. */
                void *packet_copy = malloc(PACKET_SIZE);
                assert(packet_copy);
                
                memcpy(packet_copy, traffic[i], PACKET_SIZE);
                
                queue_enqueue(
                    network_switch->voqs[i][output_port],
                    packet_copy
                );
            };

            free(traffic[i]);
        };
    };

    /*  Invoke scheduler. */
    port_num_t *port_mapping =
        (port_num_t *) malloc(
            sizeof(port_num_t) * network_switch->num_ports
        );
    
    char *map_active =
        (char *) malloc(sizeof(char) * network_switch->num_ports);
    
    iSLIP_schedule(network_switch, port_mapping, map_active);

    /*  Output on chosen ports. */
    int i;
    for (i = 0; i < network_switch->num_ports; i++) {
        host_desc_t host_out;
        int res =
            host_table_host_lookup(network_switch->host_table, i, &host_out);

        if (res) {
            assert(host_out.send);

            int j;
            for (j = 0; j < network_switch->num_ports; j++) {
                if (port_mapping[j] == i) {
                    void *out_packet =
                        queue_dequeue(network_switch->voqs[j][i]);
                    host_out.send(&host_out, out_packet);
                    free(out_packet);  
                };
            };
        };
    };

    free(port_mapping);
    free(map_active);
};

/*  API implementation. */
i_cycle_sim_switch_t cb_ib_voqs_iSLIP_cycle_switch() {
    i_cycle_sim_switch_t cycle_switch;
    cycle_switch.create = cb_ib_voqs_iSLIP_create;
    cycle_switch.free = cb_ib_voqs_iSLIP_free;
    cycle_switch.register_host =
        cb_ib_voqs_iSLIP_cycle_switch_register_host;
    cycle_switch.deregister_host =
        cb_ib_voqs_iSLIP_cycle_switch_deregister_host;
    cycle_switch.tick = NULL;

    return cycle_switch;
};