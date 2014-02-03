/*
 * 6LoWPAN MAC - layer 2 implementations
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowmac.c
 * @brief   6lowpan link layer functions
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */


#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "thread.h"
#include "msg.h"
#include "radio/radio.h"
#include "net_if.h"
#include "sixlowpan/mac.h"

#include "ip.h"
#include "icmp.h"
#include "lowpan.h"
#include "ieee802154_frame.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define RADIO_STACK_SIZE            (KERNEL_CONF_STACKSIZE_MAIN)
#define RADIO_RCV_BUF_SIZE          (64)
#define RADIO_SENDING_DELAY         (1000)

#define DEFAULT_IEEE_802154_PAN_ID  (0x1234)
#define IEEE802154_TRANSCEIVER      (TRANSCEIVER_AT86RF231 | TRANSCEIVER_CC2420 | TRANSCEIVER_MC1322X)

char radio_stack_buffer[RADIO_STACK_SIZE];
msg_t msg_q[RADIO_RCV_BUF_SIZE];

uint8_t buf[PAYLOAD_SIZE];
static uint8_t macdsn;

static inline void mac_short_to_eui64(net_if_eui64_t *eui64,
                                      uint16_t short_addr, uint16_t pan_id)
{
    eui64->uint16[0] = pan_id;
    eui64->uint16[1] = HTONS(0x00FF);
    eui64->uint16[2] = HTONS(0xFE00);
    eui64->uint16[3] = short_addr;
}

void recv_ieee802154_frame(void)
{
    msg_t m;
#if (defined(MODULE_AT86RF231) | \
     defined(MODULE_CC2420) | \
     defined(MODULE_MC1322X))
    ieee802154_packet_t *p;
#else
    radio_packet_t *p;
#endif
    uint8_t hdrlen, length;
    ieee802154_frame_t frame;
    net_if_eui64_t src, dst;

    msg_init_queue(msg_q, RADIO_RCV_BUF_SIZE);

    while (1) {
        msg_receive(&m);

        if (m.type == PKT_PENDING) {
#if (defined(MODULE_AT86RF231) | \
     defined(MODULE_CC2420) | \
     defined(MODULE_MC1322X))
            p = (ieee802154_packet_t *) m.content.ptr;
            hdrlen = ieee802154_frame_read(p->frame.payload, &frame, p->frame.payload_len);
            length = p->frame.payload_len - hdrlen - IEEE_802154_FCS_LEN;
#else
            p = (radio_packet_t *) m.content.ptr;
            hdrlen = ieee802154_frame_read(p->data, &frame, p->length);
            length = p->length - hdrlen - IEEE_802154_FCS_LEN;
#endif

            if (frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
                mac_short_to_eui64(&src, *((uint16_t *)frame.src_addr),
                                   frame.src_pan_id);
            }
            else if (frame.fcf.src_addr_m == IEEE_802154_LONG_ADDR_M) {
                memcpy(&src, frame.src_addr, 8);
            }
            else {
                puts("Unknown IEEE 802.15.4 source address mode.");
                return;
            }

            if (frame.fcf.dest_addr_m == IEEE_802154_SHORT_ADDR_M) {
                mac_short_to_eui64(&dst, *((uint16_t *)frame.dest_addr),
                                   frame.dest_pan_id);
            }
            else if (frame.fcf.dest_addr_m == IEEE_802154_LONG_ADDR_M) {
                memcpy(&dst, frame.dest_addr, 8);
            }
            else {
                puts("Unknown IEEE 802.15.4 destination address mode.");
                return;
            }

            /* deliver packet to network(6lowpan)-layer */
            lowpan_read(frame.payload, length, &src, &dst);
            /* TODO: get interface ID somehow */

            p->processing--;
        }
        else if (m.type == ENOBUFFER) {
            puts("Transceiver buffer full");
        }
        else {
            puts("Unknown packet received");
        }
    }
}

void set_ieee802154_fcf_values(ieee802154_frame_t *frame, uint8_t dest_mode,
                               uint8_t src_mode)
{
    frame->fcf.frame_type = IEEE_802154_DATA_FRAME;
    frame->fcf.sec_enb = 0;
    frame->fcf.frame_pend = 0;
    frame->fcf.ack_req = 0;
    frame->fcf.panid_comp = frame->dest_pan_id == frame->src_pan_id;
    frame->fcf.frame_ver = 0;
    frame->fcf.src_addr_m = src_mode;
    frame->fcf.dest_addr_m = dest_mode;
#if ENABLE_DEBUG
    ieee802154_frame_print_fcf_frame(frame);
#endif
}

void set_ieee802154_frame_values(int if_id, uint16_t dest_pan,
                                 ieee802154_frame_t *frame)
{
    int32_t pan_id = net_if_get_pan_id(if_id);
    // TODO: addresse aus ip paket auslesen und in frame einfuegen

    if (pan_id < 0) {
        frame->dest_pan_id = NTOLES(dest_pan);
        frame->src_pan_id = HTOLES(DEFAULT_IEEE_802154_PAN_ID);
    }
    else {
        frame->dest_pan_id = NTOLES(dest_pan);
        frame->src_pan_id = HTOLES((uint16_t)pan_id);
    }

    frame->seq_nr = macdsn;
    macdsn++;
}

int sixlowpan_mac_prepare_ieee802144_frame(
    ieee802154_frame_t *frame, int if_id, uint16_t dest_pan, const void *dest,
    uint8_t dest_len, const void *payload, uint8_t length, uint8_t mcast)
{
    uint8_t src_mode = net_if_get_src_address_mode(if_id);
    uint8_t dest_mode;
    uint16_t *fcs;
    set_ieee802154_frame_values(if_id, dest_pan, frame);

    if (dest_len == 8) {
        dest_mode = IEEE_802154_LONG_ADDR_M;
    }
    else if (dest_len == 2) {
        dest_mode = IEEE_802154_SHORT_ADDR_M;
    }
    else {
        DEBUG("Illegal IEEE 802.15.4 address mode: %d\n", dest_mode);
        return -1;
    }

    set_ieee802154_fcf_values(frame, dest_mode, src_mode);


    if (src_mode == IEEE_802154_LONG_ADDR_M) {
        net_if_get_eui64((net_if_eui64_t *)&frame->src_addr[0], if_id, 0);
    }
    else if (src_mode == IEEE_802154_SHORT_ADDR_M) {
        uint16_t src = HTONS(net_if_get_hardware_address(if_id));
        memcpy(&frame->src_addr[0], &src, 2);
    }
    else {
        DEBUG("Illegal IEEE 802.15.4 address mode: %d\n", src_mode);
        return -1;
    }

    if (mcast) {
        memset(&frame->dest_addr[0], 0, dest_len);
    }
    else {
        memcpy(&frame->dest_addr[0], dest, dest_len);
    }

    frame->payload = (uint8_t *)payload; // payload won't be changed so cast is legal.
    frame->payload_len = length;
    uint8_t hdrlen = ieee802154_frame_get_hdr_len(frame);

    memset(&buf, 0, PAYLOAD_SIZE);
    ieee802154_frame_init(frame, (uint8_t *)&buf);
    memcpy(&buf[hdrlen], frame->payload, frame->payload_len);
    /* set FCS */
    /* RSSI = 0 */
    /* buf[frame->payload_len + hdrlen] = 0; */
    /* FCS Valid = 1 / LQI Correlation Value = 0 */
    fcs = (uint16_t *)&buf[frame->payload_len + hdrlen];
    *fcs = ieee802154_frame_get_fcs(buf, frame->payload_len + hdrlen);
    DEBUG("IEEE802.15.4 frame - FCF: %02X %02X DPID: %02X SPID: %02X DSN: %02X\n",
          buf[0], buf[1], frame->dest_pan_id, frame->src_pan_id, frame->seq_nr);

    return hdrlen;
}

int sixlowpan_mac_send_data(int if_id,
                            const void *dest, uint8_t dest_len,
                            const void *payload,
                            uint8_t payload_len, uint8_t mcast)
{
    if (mcast) {
        return net_if_send_packet_broadcast(IEEE_802154_SHORT_ADDR_M,
                                            &payload,
                                            payload_len);
    }
    else {
        if (dest_len == 8) {
            return net_if_send_packet_long(if_id, (net_if_eui64_t *) &dest,
                                           payload, (size_t)payload_len);
        }
        else if (dest_len == 2) {
            return net_if_send_packet(if_id, NTOHS(*((uint16_t *)dest)),
                                      payload, (size_t)payload_len);
        }
    }

    return -1;
}

int sixlowpan_mac_send_ieee802154_frame(int if_id,
                                        const void *dest, uint8_t dest_len,
                                        const void *payload,
                                        uint8_t payload_len, uint8_t mcast)
{
    if (net_if_get_interface(if_id) &&
        net_if_get_interface(if_id)->transceivers & IEEE802154_TRANSCEIVER) {
        return sixlowpan_mac_send_data(if_id, dest, dest_len, payload,
                                       payload_len, mcast);
    }
    else {
        ieee802154_frame_t frame;
        uint16_t dest_pan = HTONS(0xabcd);
        uint8_t length;
        int hdrlen = sixlowpan_mac_prepare_ieee802144_frame(&frame, if_id,
                     dest_pan, dest, dest_len, payload, payload_len, mcast);

        if (hdrlen < 0) {
            return -1;
        }

        length = hdrlen + frame.payload_len + IEEE_802154_FCS_LEN;

        return sixlowpan_mac_send_data(if_id, dest, dest_len, buf, length,
                                       mcast);
    }
}

int sixlowpan_mac_init(void)
{
    int recv_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE,
                                 PRIORITY_MAIN - 2, CREATE_STACKTEST, recv_ieee802154_frame , "radio");
    int if_id = -1;

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        net_if_register(if_id, recv_pid);
    }

    macdsn = rand() % 256;

    return recv_pid;
}
