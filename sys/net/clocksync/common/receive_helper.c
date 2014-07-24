/**
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @{
 * @file    receive_helper.c
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#include "msg.h"
#include "radio/radio.h"
#include "net_if.h"
#include "thread.h"
#include "sixlowpan/dispatch_values.h"
#include "ieee802154_frame.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#ifdef MODULE_GTSP
#include "clocksync/gtsp.h"
#endif
#ifdef MODULE_FTSP
#include "clocksync/ftsp.h"
#endif
#ifdef MODULE_PULSESYNC
#include "clocksync/pulsesync.h"
#endif
#ifdef MODULE_CLOCKSYNC_EVAL
#include "clocksync/clocksync_eval.h"
#endif

#define RADIO_STACK_SIZE            (KERNEL_CONF_STACKSIZE_MAIN)
#define RADIO_RCV_BUF_SIZE          (64)

char radio_stack_buffer[RADIO_STACK_SIZE];
msg_t msg_q[RADIO_RCV_BUF_SIZE];
static kernel_pid_t thread_id = 0;

static void *recv_ieee802154_frame(void *arg)
{
    puts("started recv thread");
    (void) arg;

    msg_t m;
    radio_packet_t *p;
    ieee802154_frame_t frame;

    msg_init_queue(msg_q, RADIO_RCV_BUF_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
            p = (radio_packet_t *) m.content.ptr;
            ieee802154_frame_read(p->data, &frame, p->length);
            uint8_t dispatch_header = frame.payload[0];

            if ((dispatch_header >> 6) & VALID_LOWPAN_DISPATCH_HEADER) {
                DEBUG("sixlowpan packet received");
            }

#ifdef MODULE_GTSP
            else if (dispatch_header == GTSP_PROTOCOL_DISPATCH) {
                DEBUG("gtsp packet received");
                gtimer_timeval_t gtimer_toa = p->toa;
                gtsp_mac_read(frame.payload, p->src, &gtimer_toa);
            }

#endif
#ifdef MODULE_FTSP
            else if (dispatch_header == FTSP_PROTOCOL_DISPATCH) {
                DEBUG("ftsp packet received");
                gtimer_timeval_t gtimer_toa = p->toa;
                ftsp_mac_read(frame.payload, p->src, &gtimer_toa);
            }

#endif
#ifdef MODULE_PULSESYNC
            else if (dispatch_header == PULSESYNC_PROTOCOL_DISPATCH) {
                DEBUG("pulsesync packet received");
                gtimer_timeval_t gtimer_toa = p->toa;
                pulsesync_mac_read(frame.payload, p->src, &gtimer_toa);
            }

#endif
#ifdef MODULE_CLOCKSYNC_EVAL
            else if (dispatch_header == CLOCKSYNC_EVAL_PROTOCOL_DISPATCH) {
                DEBUG("clocksync_eval packet received");
                gtimer_timeval_t gtimer_toa = p->toa;
                clocksync_eval_mac_read(frame.payload, p->src, &gtimer_toa);
            }

#endif
            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            DEBUG("Transceiver buffer full");
        }
        else {
            DEBUG("Unknown packet received");
        }
    }

    return NULL;
}

void clocksync_common_init_recv(void)
{
    if (!thread_id) {
        thread_id = thread_create(radio_stack_buffer, RADIO_STACK_SIZE,
                                  PRIORITY_MAIN - 2, CREATE_STACKTEST, recv_ieee802154_frame, NULL,
                                  "clocksync_recv");

        int if_id = -1;

        while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
            net_if_register(if_id, thread_id);
        }
    }
}
