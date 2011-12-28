#include <stdint.h>
#include <stdio.h>
#include <radio/radio.h>
#include <radio/types.h>
#include <string.h>
#include <msg.h>
#include <cc1100.h>
#include <cib.h>
#include <thread.h>

#include <board.h>
#include <debug.h>

#include "protocol_msg_gateway.h"
#include "weather_protocol.h"

#define NUM_PROTOCOL_HANDLER_PIDS 8

static uint16_t protocol_handler_pid;

uint16_t pkts_received = 0;

packet_t packet_buffer[PACKET_BUFFER_SIZE];

static void protocol_msg_gateway(void* payload, int msg_size, packet_info_t* packet_info) {
    int n = -1;
    extern cib_t cib;

    pkts_received++;
    if (!cc1100_get_address()) {
        puts("No address configured, not processing incoming packet");
        return;
    }

    if (protocol_handler_pid <= 0) {
        puts("protocol_handler(): received packet without protocol handler. msg dropped.");
        return;
    }
/*
    int mypos = packet_buffer_next++;
    if (packet_buffer_next == PACKET_BUFFER_SIZE) packet_buffer_next = 0;
*/
    n = cib_put(&cib);

    if (n != -1) {
        packet_t *p = &(packet_buffer[n]);
        p->packet_info = *packet_info;
        p->msg_size = msg_size;
        memcpy(p->payload, payload, msg_size);
    }
    else {
        puts("Buffer full!");
        return;
    }


/*
    m.type = 0;
    m.content.value = mypos;
    int success = msg_send_int(&m, protocol_handler_pid);
    if (! success) {
        // should set timer to retry. Dropping pkt for now.
        DEBUG("protocol_handler(): msg dropped.");
    }
*/

    thread_wakeup(protocol_handler_pid);
}

void init_protocol_msg_gateway() {
    puts("Init protocol msg gateway");
    cc1100_set_packet_handler(WEATHER_PROTOCOL_NR, protocol_msg_gateway);
}

int set_protocol_handler_thread(int pid) {
    protocol_handler_pid = pid;
    return 0;
}
