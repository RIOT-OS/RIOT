/*
 * Copyright (C) 2013, Igor Merkulow <igor.merkulow@gmail.com>
 * Copyright (C) 2014, Oliver Hahm <oliver.hahm@inria.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @author Igor Merkulow <igor.merkulow@gmail.com>
 * @author Oliver Hahm <oliver.hahm@inria.fr>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "thread.h"
#include "msg.h"
#include "mutex.h"

#include "transceiver.h"
#include "radio/types.h"
#include "vtimer.h"
#include "timex.h"
#include "l2_ping.h"

#include "debug.h"

/*--------------------------------------------------------------------------------------*/
/* interal defines */
#define RCV_BUFFER_SIZE     (64)
#define RADIO_STACK_SIZE    (KERNEL_CONF_STACKSIZE_DEFAULT)
#define MAX_PROB_STATS      (64)

/* internal prototypes */
/* link layer send function for l2_pings */
static int8_t send_l2_packet(radio_address_t dst, l2_ping_type_t type, uint16_t seq, const char *payload, uint8_t payload_len);
/* handler for incoming packets from transceiver */
static void *l2_pkt_handler(void *unused);
/* handles l2 ping requests */
static void ping_handler(radio_address_t addr, uint16_t seq);
/* handles l2 pong responses */
static void pong_handler(void);
/* handle l2 probes */
static void probe_handler(radio_address_t src);
/* calculates the round trip time for a ping-pong exchange */
static void calc_rtt(void);

/* internal buffers and variables */
static char l2_pkt_handler_stack_buffer[RADIO_STACK_SIZE];
static msg_t msg_q[RCV_BUFFER_SIZE];

#ifndef DISABLE_PROB_STATS
static l2_probe_stat_entry_t probe_stats[MAX_PROB_STATS];
#endif

static timex_t start, end, rtt_sum;
static uint8_t ping_sent;
static struct mutex_t ping_sender_mutex;
l2_ping_stats_t l2_ping_stats;

/*--------------------------------------------------------------------------------------*/
/* public interface functions */
void l2_ping_init(void)
{
    mutex_init(&ping_sender_mutex);
    kernel_pid_t l2_pkt_handler_pid = thread_create(l2_pkt_handler_stack_buffer,
                                                    RADIO_STACK_SIZE,
                                                    PRIORITY_MAIN - 2,
                                                    CREATE_STACKTEST,
                                                    l2_pkt_handler, NULL,
                                                    "l2_pkt_handler");
    uint16_t transceivers = TRANSCEIVER_DEFAULT;

#ifndef MODULE_NET_IF
    transceiver_init(transceivers);
    (void) transceiver_start();
#endif
    transceiver_register(transceivers, l2_pkt_handler_pid);
}

void l2_ping(radio_address_t addr, uint16_t count, uint32_t interval,
             const char *payload, uint8_t payload_len, uint8_t probe_only)
{
    l2_ping_type_t pt;

    probe_only ? (pt = L2_PROBE) : (pt = L2_PING);

    if (!interval) {
        interval = L2_PING_DEFAULT_INTERVAL;
    }

    mutex_lock(&ping_sender_mutex);
    l2_ping_stats.dst = addr;
    l2_ping_stats.ping_count = 0;
    l2_ping_stats.pong_count = 0;
    l2_ping_stats.last_rtt = timex_set(0, 0);
    l2_ping_stats.avg_rtt = timex_set(0, 0);
    l2_ping_stats.max_rtt = timex_set(0, 0);
    l2_ping_stats.min_rtt = timex_set(UINT32_MAX, UINT32_MAX);

    for (unsigned i = 1; (count == 0) || (i <= count); i++) {
        vtimer_now(&start);

        if (send_l2_packet(addr, pt, i, payload, payload_len)) {
            ping_sent = 1;
            l2_ping_stats.ping_count++;
        }
        if ((!count) || (i <= count)) {
            vtimer_usleep(interval);
        }
    }
    mutex_unlock(&ping_sender_mutex);
}

void l2_probe_stats(l2_probe_stat_entry_t *l2_probe_stats[], uint16_t *count)
{
    unsigned i;
    *l2_probe_stats = probe_stats;
    for (i = 0; i < MAX_PROB_STATS; i++) {
        if (!(probe_stats[i]).src) {
            break;
        }
    }
    *count = i;
}

/*--------------------------------------------------------------------------------------*/
/* internal functions */
static void *l2_pkt_handler(void *unused)
{
    (void) unused;

    msg_t m;
    radio_packet_t *p;
    l2_ping_payload_t *pp;

    msg_init_queue(msg_q, sizeof(msg_q));

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            vtimer_now(&end);
            p = (radio_packet_t *) m.content.ptr;
            pp = (l2_ping_payload_t *) p->data;

            if ((pp->type & L2_PAYLOAD_TYPE) == L2_PAYLOAD_PING) {
                DEBUGF("INFO: received l2_ping_packet number %d from %d with payload(%d) %.*s.\n",
                        pp->seq, p->src, pp->payload_len, pp->payload_len, pp->payload);
                switch (pp->type & L2_PING_TYPE) {
                    case L2_PING:
                        ping_handler(p->src, pp->seq);
                        break;
                    case L2_PONG:
                        pong_handler();
                        break;
                    case L2_PROBE:
                        probe_handler(p->src);
                        break;
                    default:
                        DEBUGF("ERROR: Unknown L2 PING type\n");
                }
            }
            else {
                DEBUGF("WARN: no L2 ping packet, type is %02X\n", pp->type);
            }

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            DEBUGF("ERROR: Transceiver buffer full\n");
        }
        else {
            DEBUGF("ERROR: Unknown messagereceived\n");
        }
    }

    return NULL;
}

static void calc_rtt(void)
{
    timex_t rtt = timex_sub(end, start);
    rtt_sum = timex_add(rtt_sum, rtt);

    l2_ping_stats.last_rtt = rtt;
    l2_ping_stats.avg_rtt = timex_from_uint64(timex_uint64(rtt_sum) / l2_ping_stats.pong_count);
    if (timex_cmp(rtt, l2_ping_stats.max_rtt) > 0) {
        l2_ping_stats.max_rtt = rtt;
    }
    if (timex_cmp(rtt, l2_ping_stats.min_rtt) < 0) {
        l2_ping_stats.min_rtt = rtt;
    }
}

static int8_t send_l2_packet(radio_address_t dst, l2_ping_type_t type, uint16_t seq, const char *payload, uint8_t payload_len)
{
    radio_packet_t p;
    l2_ping_payload_t pp;

    if (payload_len > L2_PING_PAYLOAD_SIZE) {
        DEBUGF("ERROR: payload too big for l2 ping packet of type %u\n", type);
        return -1;
    }

    transceiver_command_t tcmd;
    tcmd.transceivers = TRANSCEIVER_DEFAULT;
    tcmd.data = &p;

    pp.type = type | L2_PAYLOAD_PING;
    pp.seq = seq;

    memset(pp.payload, 0, L2_PING_PAYLOAD_SIZE);
    memcpy(pp.payload, payload, payload_len);
    pp.payload_len = payload_len;

    p.data = (uint8_t*) &pp;
    p.length = sizeof(pp);
    p.dst = dst;

    msg_t mesg;
    mesg.type = SND_PKT;
    mesg.content.ptr = (char *) &tcmd;

    msg_send_receive(&mesg, &mesg, transceiver_pid);
    int8_t response = mesg.content.value;

    if (response <= 0) {
        DEBUGF("ERROR: sending L2 packet of type %x failed.\n", type);
    }

    return response;
}

static void ping_handler(radio_address_t addr, uint16_t seq)
{
    send_l2_packet(addr, L2_PONG, seq, NULL, 0);
}

static void pong_handler(void)
{
    if (ping_sent) {
        ping_sent = 0;
        l2_ping_stats.pong_count++;
        calc_rtt();
    }
    else {
        DEBUGF("ERROR: received pong without a sent ping\n");
    }
}

static void probe_handler(radio_address_t src)
{
#ifdef DISABLE_PROB_STATS
    DEBUGF("WARN: L2 probe statistics are disabled, not handling probe packet.\n");
#else
    unsigned i;
    for (i = 0; i < MAX_PROB_STATS; i++) {
        /* found entry for this source address */
        if (probe_stats[i].src == src) {
            probe_stats[i].count++;
            return;
        }
        /* found empty entry */
        else if (!(probe_stats[i].src)) {
            probe_stats[i].src = src;
            probe_stats[i].count++;
            return;
        }
    }
    DEBUGF("ERROR: L2 probe statistics full! Probe data from %u not stored.\n", src);
#endif
}
