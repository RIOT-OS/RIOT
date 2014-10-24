/*
 * Copyright (C) 2014  Philipp Rosenkranz, Daniel Jentsch
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @{
 * @file    send_helper.c
 * @author  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
 * @author  Daniel Jentsch <d.jentsch@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "transceiver.h"
#include "ieee802154_frame.h"
#include "net_if.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define DEFAULT_IEEE_802154_PAN_ID (0x1234)

static uint8_t macdsn = 0;

static void set_ieee802154_frame_values(int if_id, uint16_t dest_pan,
        ieee802154_frame_t *frame);
static void set_ieee802154_fcf_values(ieee802154_frame_t *frame,
        uint8_t dest_mode, uint8_t src_mode);

void clocksync_common_send_broadcast(uint8_t *send_buf, void *beacon,
        size_t beacon_len)
{
    ieee802154_frame_t frame;
    uint16_t frame_dest_pan = HTONS(0xabcd);
    uint8_t if_id = 0;
    uint8_t frame_dest_len = 8;
    uint8_t frame_length = 0;
    uint8_t frame_hdrlen = 0;
    uint16_t *frame_fcs;

    set_ieee802154_frame_values(if_id, frame_dest_pan, &frame);
    set_ieee802154_fcf_values(&frame, IEEE_802154_LONG_ADDR_M,
                              IEEE_802154_LONG_ADDR_M);
    net_if_get_eui64((net_if_eui64_t *) &(frame.src_addr), if_id, 0);
    memset(&(frame.dest_addr), 0xff, frame_dest_len);

    frame.payload = beacon; // payload won't be changed so cast is legal.
    frame.payload_len = beacon_len;
    frame_hdrlen = ieee802154_frame_get_hdr_len(&frame);

    memset(send_buf, 0, PAYLOAD_SIZE);
    ieee802154_frame_init(&frame, send_buf);
    memcpy(&send_buf[frame_hdrlen], frame.payload, frame.payload_len);
    /* set FCS */
#ifdef MODULE_CC110X_NG
    frame_fcs = (uint16_t *)&send_buf[frame.payload_len + frame_hdrlen+1];
#else
    frame_fcs = (uint16_t *) &send_buf[frame.payload_len + frame_hdrlen];
#endif
    *frame_fcs = ieee802154_frame_get_fcs(send_buf,
                                          frame.payload_len + frame_hdrlen);
    DEBUG("IEEE802.15.4 frame - FCF: %02X %02X DPID: %02X SPID: %02X DSN: %02X\n",
          send_buf[0], send_buf[1], frame.dest_pan_id, frame.src_pan_id,
          frame.seq_nr);

    frame_length = frame_hdrlen + frame.payload_len + IEEE_802154_FCS_LEN;
    net_if_send_packet_broadcast(IEEE_802154_SHORT_ADDR_M, send_buf,
            frame_length);
}

static void set_ieee802154_frame_values(int if_id, uint16_t dest_pan,
        ieee802154_frame_t *frame)
{
    int32_t pan_id = net_if_get_pan_id(if_id);
    // TODO: addresse aus ip paket auslesen und in frame einfuegen

    if (pan_id < 0)
    {
        frame->dest_pan_id = NTOLES(dest_pan);
        frame->src_pan_id = HTOLES(DEFAULT_IEEE_802154_PAN_ID);
    }
    else
    {
        frame->dest_pan_id = NTOLES(dest_pan);
        frame->src_pan_id = HTOLES((uint16_t)pan_id);
    }

    frame->seq_nr = macdsn;
    macdsn++;
}

static void set_ieee802154_fcf_values(ieee802154_frame_t *frame,
        uint8_t dest_mode, uint8_t src_mode)
{
    frame->fcf.frame_type = IEEE_802154_DATA_FRAME;
    frame->fcf.sec_enb = 0;
    frame->fcf.frame_pend = 0;
    frame->fcf.ack_req = 0;
    frame->fcf.panid_comp = (frame->dest_pan_id == frame->src_pan_id);
    frame->fcf.frame_ver = 0;
    frame->fcf.src_addr_m = src_mode;
    frame->fcf.dest_addr_m = dest_mode;
}
