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

void clocksync_common_send_broadcast(uint8_t *send_buf, void *beacon,
                                     size_t beacon_len)
{
    ieee802154_frame_t frame;

    memset(send_buf, 0, PAYLOAD_SIZE);
    frame.fcf.frame_type = IEEE_802154_DATA_FRAME;
    frame.fcf.sec_enb = 0;
    frame.fcf.frame_pend = 0;
    frame.fcf.ack_req = 0;
    frame.fcf.panid_comp = (frame.dest_pan_id == frame.src_pan_id);
    frame.fcf.frame_ver = 0;
    frame.fcf.src_addr_m = net_if_get_src_address_mode(0);
    frame.fcf.dest_addr_m = IEEE_802154_LONG_ADDR_M;
    memset(&frame.dest_addr[0], 0xff, 8);

    net_if_get_eui64((net_if_eui64_t *) &frame.src_addr[0], 0, 0);

    frame.payload = beacon;
    frame.payload_len = beacon_len;
    uint8_t hdrlen = ieee802154_frame_get_hdr_len(&frame);
    ieee802154_frame_init(&frame, (uint8_t *) send_buf);
    memcpy(&send_buf[hdrlen], frame.payload, frame.payload_len);
    net_if_send_packet_broadcast(0, send_buf,
                                 hdrlen + beacon_len + IEEE_802154_FCS_LEN);
}
