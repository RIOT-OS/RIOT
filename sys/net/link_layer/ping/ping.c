/**
 * Ping: low level ping pong
 *
 * Copyright (C) 2013, Igor Merkulow <igor.merkulow@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file
 * @author Igor Merkulow <igor.merkulow@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "msg.h"

#include "transceiver.h"
#include "radio/types.h"
#include "vtimer.h"
#include "timex.h"
#include "ping.h"

static void send_l2_packet(radio_address_t dst, l2_ping_type_t type);
static void pong(radio_address_t src);
static void *pkt_handler(void *unused);
static void ping_handler(packet_info_t *packet_info);
static void pong_handler(void);
static void print_success(void);
static void print_failed(void);
static void calc_rtt(void);

char radio_stack_buffer[RADIO_STACK_SIZE];
msg_t msg_q[RCV_BUFFER_SIZE];

ping_payload pipa;
timex_t start;
float rtt = 0;

void ping_handler(packet_info_t *packet_info)
{
    pong(packet_info->phy_src);
}

void ping_init(void)
{
    kernel_pid_t radio_pid = thread_create(radio_stack_buffer,
                                           RADIO_STACK_SIZE, PRIORITY_MAIN - 2,
                                           CREATE_STACKTEST, pkt_handler, NULL,
                                           "l2_ping_handler");
    uint16_t transceivers = TRANSCEIVER_DEFAULT;

    transceiver_init(transceivers);
    (void) transceiver_start();
    transceiver_register(transceivers, radio_pid);
}

void ping(radio_address_t addr)
{
    while (1) {
        vtimer_now(&start);

        send_l2_packet(addr, L2_PING);
        vtimer_usleep(500 * 1000);
    }
}

static void pkt_handler(void)
{
    (void) unused;

    msg_t m;
    radio_packet_t *p;

    msg_init_queue(msg_q, sizeof(msg_q));

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }

    return NULL;
}

static void calc_rtt(void)
{
    timex_t end;
    vtimer_now(&end);
    timex_t result = timex_sub(end, start);

    rtt = result.seconds + (float)result.microseconds / (1000.0 * 1000.0);
}

static void print_success(void)
{
    printf("%s%f%s\n", "time=", rtt, "ms");
}

static void print_failed(void)
{
    printf("%s\n", "ping failed");
}

static void pong(radio_address_t src)
{
    send_l2_packet(src, L2_PONG);
}

static void send_l2_packet(radio_address_t dst, l2_ping_type_t type)
{
    radio_packet_t p;

    transceiver_command_t tcmd;
    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &p;

    pipa.type = type;

    p.data = (uint8_t*) &pipa;
    p.length = sizeof(pipa);
    p.dst = dst;

    msg_t mesg;
    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    int8_t response = mesg.content.value;

    if (response <= 0) {
        print_failed();
    }
}

static void pong_handler(void)
{
    calc_rtt();
    print_success();
}
