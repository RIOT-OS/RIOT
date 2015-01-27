/*
 * 6LoWPAN MAC - layer 2 implementations
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowmac.c
 * @brief   6lowpan link layer functions
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include "ieee802154.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define RADIO_STACK_SIZE            (KERNEL_CONF_STACKSIZE_MAIN)
#define RADIO_RCV_BUF_SIZE          (64)
#define RADIO_SENDING_DELAY         (1000)

#define DEFAULT_IEEE_802154_PAN_ID  (0x1234)

static char radio_stack_buffer[RADIO_STACK_SIZE];
static msg_t msg_q[RADIO_RCV_BUF_SIZE];

static uint8_t lowpan_mac_buf[PAYLOAD_SIZE];
static uint8_t macdsn;

static inline void mac_frame_short_to_eui64(net_if_eui64_t *eui64,
                                            uint8_t *frame_short)
{
    /* Since this is a short address, which is never globally unique, we set
     * the local/universal bit to 1. */
    eui64->uint32[0] = HTONL(0x020000ff);
    eui64->uint16[2] = HTONS(0xfe00);
    eui64->uint8[6] = frame_short[1];
    eui64->uint8[7] = frame_short[0];
}

static void *recv_ieee802154_frame(void *arg)
{
    (void) arg;

    msg_t m;
#if (defined(MODULE_AT86RF231) | \
     defined(MODULE_CC2420) | \
     defined(MODULE_MC1322X))
    ieee802154_packet_t *p;
#else
    radio_packet_t *p;
    uint8_t hdrlen;
#endif
    uint8_t length;
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
            memcpy(&frame, &p->frame, sizeof(ieee802154_frame_t));
            length = p->frame.payload_len;
#else
            p = (radio_packet_t *) m.content.ptr;
            hdrlen = ieee802154_frame_read(p->data, &frame, p->length);
            length = p->length - hdrlen - IEEE_802154_FCS_LEN;
#endif

#if ENABLE_DEBUG
            DEBUG("INFO: Received IEEE 802.15.4. packet (length = %d):\n", length);
            DEBUG("INFO: FCF:\n");
            ieee802154_frame_print_fcf_frame(&frame);

            DEBUG("Sender:");

            for (uint8_t i = 0; i < 8; i++) {
                printf("%02x ", frame.src_addr[i]);
            }

            DEBUG("\n");

            DEBUG("Receiver:");

            for (size_t i = 0; i < 8; i++) {
                printf("%02x ", frame.dest_addr[i]);
            }

            DEBUG("\n");

            DEBUG("Payload:\n");

            for (uint8_t i = 0; i < frame.payload_len; i++) {
                printf("%02x ", frame.payload[i]);

                if (!((i + 1) % 16) || i == frame.payload_len - 1) {
                    printf("\n");
                }
            }

#endif

            if (frame.fcf.src_addr_m == IEEE_802154_SHORT_ADDR_M) {
                mac_frame_short_to_eui64(&src, frame.src_addr);
            }
            else if (frame.fcf.src_addr_m == IEEE_802154_LONG_ADDR_M) {
                memcpy(&src, frame.src_addr, 8);
            }
            else {
                DEBUG("Unknown IEEE 802.15.4 source address mode.\n");
                p->processing--;
                continue;
            }

            if (frame.fcf.dest_addr_m == IEEE_802154_SHORT_ADDR_M) {
                mac_frame_short_to_eui64(&dst, frame.dest_addr);
            }
            else if (frame.fcf.dest_addr_m == IEEE_802154_LONG_ADDR_M) {
                memcpy(&dst, frame.dest_addr, 8);
            }
            else {
                DEBUG("Unknown IEEE 802.15.4 destination address mode.\n");
                p->processing--;
                continue;
            }

            /* deliver packet to network(6lowpan)-layer */
            lowpan_read(frame.payload, length, &src, &dst);
            /* TODO: get interface ID somehow */

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

void set_ieee802154_fcf_values(ieee802154_frame_t *frame, uint8_t dest_mode,
                               uint8_t src_mode)
{
    frame->fcf.frame_type = IEEE_802154_DATA_FRAME;
    frame->fcf.sec_enb = 0;
    frame->fcf.frame_pend = 0;
    frame->fcf.ack_req = 0;
    frame->fcf.panid_comp = (frame->dest_pan_id == frame->src_pan_id);
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
        DEBUG("Illegal IEEE 802.15.4 address for address length %d\n", dest_len);
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
        memset(&frame->dest_addr[0], 0xff, dest_len);
    }
    else {
        memcpy(&frame->dest_addr[0], dest, dest_len);
    }

    frame->payload = (uint8_t *)payload; // payload won't be changed so cast is legal.
    frame->payload_len = length;
    uint8_t hdrlen = ieee802154_frame_get_hdr_len(frame);

    memset(&lowpan_mac_buf, 0, PAYLOAD_SIZE);
    ieee802154_frame_init(frame, (uint8_t *)&lowpan_mac_buf);
    memcpy(&lowpan_mac_buf[hdrlen], frame->payload, frame->payload_len);
    /* set FCS */
#if (defined(MODULE_CC110X) || defined(MODULE_CC110X_LEGACY))
    fcs = (uint16_t *)&lowpan_mac_buf[frame->payload_len + hdrlen+1];
#else
    fcs = (uint16_t *)&lowpan_mac_buf[frame->payload_len + hdrlen];
#endif
    *fcs = ieee802154_frame_get_fcs(lowpan_mac_buf, frame->payload_len + hdrlen);
    DEBUG("IEEE802.15.4 frame - FCF: %02X %02X DPID: %02X SPID: %02X DSN: %02X\n",
          lowpan_mac_buf[0], lowpan_mac_buf[1], frame->dest_pan_id,
          frame->src_pan_id, frame->seq_nr);

    return hdrlen;
}

int sixlowpan_mac_send_data(int if_id,
                            const void *dest, uint8_t dest_len,
                            const void *payload,
                            uint8_t payload_len, uint8_t mcast)
{
    if (mcast) {
        return net_if_send_packet_broadcast(IEEE_802154_SHORT_ADDR_M,
                                            payload,
                                            payload_len);
    }
    else {
        if (dest_len == 8) {
            return net_if_send_packet_long(if_id, (net_if_eui64_t *) dest,
                                           payload, (size_t)payload_len);
        }
        else if (dest_len == 2) {
            return net_if_send_packet(if_id, NTOHS((*((net_if_eui64_t*)dest)).uint16[0]),
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
                                                            dest_pan, dest,
                                                            dest_len, payload,
                                                            payload_len, mcast);

        if (hdrlen < 0) {
            return -1;
        }

        length = hdrlen + frame.payload_len + IEEE_802154_FCS_LEN;

        return sixlowpan_mac_send_data(if_id, dest, dest_len, lowpan_mac_buf,
                                       length, mcast);
    }
}

kernel_pid_t sixlowpan_mac_init(void)
{
    kernel_pid_t recv_pid = thread_create(radio_stack_buffer, RADIO_STACK_SIZE,
                                          PRIORITY_MAIN - 2, CREATE_STACKTEST,
                                          recv_ieee802154_frame, NULL, "radio");
    int if_id = -1;

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        net_if_register(if_id, recv_pid);
    }

    macdsn = rand() % 256;

    return recv_pid;
}
