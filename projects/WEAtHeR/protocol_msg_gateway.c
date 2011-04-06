#include <stdint.h>
#include <stdio.h>
#include <radio/radio.h>
#include <radio/types.h>
#include <string.h>
#include <msg.h>
#include <cc1100.h>

#include <board.h>
#include "protocol_msg_gateway.h"

#define NUM_PROTOCOL_HANDLER_PIDS 8

static uint16_t protocol_handler_pid;

static int packet_buffer_next = 0;
packet_t packet_buffer[PACKET_BUFFER_SIZE];

static void protocol_msg_gateway(void* payload, int msg_size, protocol_t protocol, packet_info_t* packet_info) {
    msg_t m;

    if (protocol_handler_pid <= 0) {
        puts("protocol_handler(): received packet without protocol handler. msg dropped.");
        return;
    }

    int mypos = packet_buffer_next++;
    if (packet_buffer_next == PACKET_BUFFER_SIZE) packet_buffer_next = 0;

    packet_t *p = &(packet_buffer[mypos]);
    p->packet_info = *packet_info;
    p->msg_size = msg_size;
    memcpy(p->payload, payload, msg_size);

    m.type = 0;
    m.content.value = mypos;
    int success = msg_send_int(&m, protocol_handler_pid);
    if (! success) {
        /* should set timer to retry. Dropping pkt for now. */
        puts("protocol_handler(): msg dropped.");
    }
}

void init_protocol_msg_gateway() {
    puts("Init protocol msg gateway");
    cc1100_set_packet_monitor(protocol_msg_gateway);
}

int set_protocol_handler_thread(int pid) {
    protocol_handler_pid = pid;
    return 0;
}
