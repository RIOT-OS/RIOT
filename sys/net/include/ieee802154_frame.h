/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    net_ieee802154 IEEE802.15.4
 * @ingroup net
 * @brief   IEEE802.15.4 adapaption layer
 * @{
 *
 * @file    ieee802154/ieee802154_frame.h
 * @brief   IEEE 802.14.4 framing data structs and prototypes
 *
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef IEEE802154_IEEE802154_FRAME
#define IEEE802154_IEEE802154_FRAME

#include <stdio.h>
#include <stdint.h>

/* maximum 802.15.4 header length */
#define IEEE_802154_MAX_HDR_LEN         23
/* ...and FCS*/
#define IEEE_802154_FCS_LEN             2

#define IEEE_802154_BEACON_FRAME        0
#define IEEE_802154_DATA_FRAME          1
#define IEEE_802154_ACK_FRAME           2
#define IEEE_802154_MAC_CMD_FRAME       3

#define IEEE_802154_SHORT_ADDR_M        2
#define IEEE_802154_LONG_ADDR_M         3

#define IEEE_802154_PAN_ID              0x1234

typedef struct __attribute__((packed)) {
    uint8_t frame_type;
    uint8_t sec_enb;
    uint8_t frame_pend;
    uint8_t ack_req;
    uint8_t panid_comp;
    uint8_t dest_addr_m;
    uint8_t frame_ver;
    uint8_t src_addr_m;
} ieee802154_frame_fcf_frame_t;

typedef struct __attribute__((packed)) {
    ieee802154_frame_fcf_frame_t fcf;
    uint8_t seq_nr;
    uint16_t dest_pan_id;
    uint8_t dest_addr[8];
    uint16_t src_pan_id;
    uint8_t src_addr[8];
    uint8_t *payload;
    uint8_t payload_len;
} ieee802154_frame_t;

uint8_t ieee802154_frame_init(ieee802154_frame_t *frame, uint8_t *buf);
uint8_t ieee802154_frame_get_hdr_len(ieee802154_frame_t *frame);
uint8_t ieee802154_frame_read(uint8_t *buf, ieee802154_frame_t *frame, uint8_t len);
void ieee802154_frame_print_fcf_frame(ieee802154_frame_t *frame);

/** @} */
#endif /* IEEE802154_IEEE802154_FRAME */
