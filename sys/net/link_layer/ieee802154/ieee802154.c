/**
 * Copyright (C) 2013  INRIA.
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ieee802154
 * @{
 *
 * @file
 * @brief       IEEE 802.14.4 framing operations
 *
 * @author      Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @}
 */

#include <string.h>

#include "ieee802154.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define IEEE_802154_FCS_POLY    (0x8408)  /* x^16 + x^12 + x^5 + 1 for LSB first */

uint8_t ieee802154_frame_init(ieee802154_frame_t *frame, uint8_t *buf)
{
    /* Frame Control Field - 802.15.4 - 2006 - 7.2.1.1  */
    uint8_t index = 3;

    /* copy FCF and sequence number */
    memcpy(buf, frame, 3);

    /* Destination PAN Identifier - 802.15.4 - 2006 - 7.2.1.3 */
    if (frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) {
        memcpy(buf + index, &frame->dest_pan_id, 2);
        index += 2;
    }

    /* Destination Address - 802.15.4 - 2006 - 7.2.1.4 */
    if ((frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) == IEEE_802154_FCF_DST_ADDR_SHORT) {
        buf[index]     = frame->dest_addr[1];
        buf[index + 1] = frame->dest_addr[0];
        index += 2;
    }
    else if ((frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) == IEEE_802154_FCF_DST_ADDR_LONG) {
        buf[index]     = frame->dest_addr[7];
        buf[index + 1] = frame->dest_addr[6];
        buf[index + 2] = frame->dest_addr[5];
        buf[index + 3] = frame->dest_addr[4];
        buf[index + 4] = frame->dest_addr[3];
        buf[index + 5] = frame->dest_addr[2];
        buf[index + 6] = frame->dest_addr[1];
        buf[index + 7] = frame->dest_addr[0];
        index += 8;
    }

    /* Source PAN Identifier - 802.15.4 - 2006 - 7.2.1.5 */
    if (!(frame->fcf & IEEE_802154_FCF_PANID_COMP)) {
        if (frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) {
            buf[index + 1]  = ((frame->src_pan_id >> 8) & 0xff);
            buf[index]      = (frame->src_pan_id & 0xff);
            index += 2;
        }
    }

    /* Source Address field - 802.15.4 - 2006 - 7.2.1.6 */
    if ((frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) == IEEE_802154_FCF_SRC_ADDR_SHORT) {
        buf[index]     = frame->src_addr[1];
        buf[index + 1] = frame->src_addr[0];
        index += 2;
    }
    else if ((frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) == IEEE_802154_FCF_SRC_ADDR_LONG) {
        buf[index]     = frame->src_addr[7];
        buf[index + 1] = frame->src_addr[6];
        buf[index + 2] = frame->src_addr[5];
        buf[index + 3] = frame->src_addr[4];
        buf[index + 4] = frame->src_addr[3];
        buf[index + 5] = frame->src_addr[2];
        buf[index + 6] = frame->src_addr[1];
        buf[index + 7] = frame->src_addr[0];
        index += 8;
    }

#if ENABLE_DEBUG
    DEBUG("INFO: IEEE 802.15.4 header initialized:\n");

    for (size_t i = 0; i < index; i++) {
        printf("%02x ", buf[i]);

        if (!((i + 1) % 16) || i == index - 1) {
            printf("\n");
        }
    }

#endif

    return index;
}

/**     2      1      2     VAR    2      VAR
  * -------------------------------------------
  * |  FCF  | DSN | DPID  | DAD | SPID  | SAD |
  * -------------------------------------------
  */
uint8_t ieee802154_frame_get_hdr_len(ieee802154_frame_t *frame)
{
    uint8_t len = 3;    /* FCF and DSN have fixed length of 3 byte */

    if ((frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) == IEEE_802154_FCF_DST_ADDR_SHORT) {
        len += 4;       /* 2 byte short address + 2 byte destination PAN id */
    }
    else if ((frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) == IEEE_802154_FCF_DST_ADDR_SHORT) {
        len += 10;      /* 8 byte long address + 2 byte destination PAN id */
    }

    if ((frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) == IEEE_802154_FCF_SRC_ADDR_SHORT) {
        len += 4;       /* 2 byte short address + 2 byte source PAN id */
    }
    else if ((frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) == IEEE_802154_FCF_SRC_ADDR_LONG) {
        len += 10;      /* 8 byte long address + 2 byte source PAN id */
    }

    /* see if PAN id is compressed (src_pan_id := dst_pan_id) */
    if (frame->fcf & IEEE_802154_FCF_PANID_COMP) {
        len -= 2;
    }

    return len;
}

uint8_t ieee802154_frame_read(uint8_t *buf, ieee802154_frame_t *frame,
                              uint8_t len)
{
    uint8_t index = 3;

    /* read frame configuration bits */
    frame->fcf = (((uint16_t)buf[0]) << 8 ) | buf[1];
    /* read sequence number */
    frame->seq_nr = buf[2];

    /* if destination address is short or long, set destination PAN id */
    if (frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK)
    {
        frame->dest_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];
        index += 2;
    }

    /* read the destination address */
    switch (frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) {
        case IEEE_802154_FCF_DST_ADDR_EMPTY:
            DEBUG("dst_addr_m: pan identifier/address fields empty\n");
            break;
        case IEEE_802154_FCF_DST_ADDR_SHORT:
            /* read address in little-endian order */
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index + 1];
            index += 2;
            break;
        case IEEE_802154_FCF_DST_ADDR_LONG:
            /* read address in network order */
            frame->dest_addr[7] = buf[index];
            frame->dest_addr[6] = buf[index + 1];
            frame->dest_addr[5] = buf[index + 2];
            frame->dest_addr[4] = buf[index + 3];
            frame->dest_addr[3] = buf[index + 4];
            frame->dest_addr[2] = buf[index + 5];
            frame->dest_addr[1] = buf[index + 6];
            frame->dest_addr[0] = buf[index + 7];
            index += 8;
            break;
    }

    /* set source PAN id, if it was send */
    if (!(frame->fcf & IEEE_802154_FCF_PANID_COMP)) {
        if (frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) {
            frame->src_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];
            index += 2;
        }
    }

    /* parse source address */
    switch (frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) {
        case IEEE_802154_FCF_SRC_ADDR_EMPTY:
            DEBUG("src_addr_m: pan identifier/address fields empty\n");
            break;
        case IEEE_802154_FCF_SRC_ADDR_SHORT:
            /* read address in little-endian order */
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index + 1];
            index += 2;
            break;
        case IEEE_802154_FCF_SRC_ADDR_LONG:
            /* read address in network order */
            frame->src_addr[7] = buf[index];
            frame->src_addr[6] = buf[index + 1];
            frame->src_addr[5] = buf[index + 2];
            frame->src_addr[4] = buf[index + 3];
            frame->src_addr[3] = buf[index + 4];
            frame->src_addr[2] = buf[index + 5];
            frame->src_addr[1] = buf[index + 6];
            frame->src_addr[0] = buf[index + 7];
            index += 8;
            break;
    }

    /* set payload pointers */
    frame->payload = (buf + index);
    frame->payload_len = (len - index - IEEE_802154_FCS_LEN);

    /* return actual header length */
    return index;
}

/* crc with lsb first */
uint16_t ieee802154_frame_get_fcs(const uint8_t *frame, uint8_t frame_len)
{
    uint16_t r = 0;

    for (uint8_t byte = 0; byte < frame_len; ++byte) {
        r ^= frame[byte];

        for (uint8_t bit = 8; bit > 0; --bit) {
            if (r & 0x0001) {
                r = (r >> 1) ^ IEEE_802154_FCS_POLY;
            }
            else {
                r = (r >> 1);
            }
        }
    }

    return r;
}

void ieee802154_frame_print_fcf_frame(ieee802154_frame_t *frame)
{
    printf("frame type: %03x\n"
           "security enabled: %02x\n"
           "frame pending: %02x\n"
           "ack requested: %02x\n"
           "pan id compression: %02x\n"
           "destination address mode: %02x\n"
           "frame version: %02x\n"
           "source address mode: %02x\n",
           (frame->fcf & IEEE_802154_FCF_TYPE_MASK) >> IEEE_802154_FCF_TYPE_POS,
           (frame->fcf & IEEE_802154_FCF_SEC_EN) >> IEEE_802154_FCF_SEC_EN_POS,
           (frame->fcf & IEEE_802154_FCF_FRAME_PEND) >> IEEE_802154_FCF_FRAME_PEND_POS,
           (frame->fcf & IEEE_802154_FCF_ACK_REQ) >> IEEE_802154_FCF_ACK_REQ_POS,
           (frame->fcf & IEEE_802154_FCF_PANID_COMP) >> IEEE_802154_FCF_PANID_COMP_POS,
           (frame->fcf & IEEE_802154_FCF_DST_ADDR_MASK) >> IEEE_802154_FCF_DST_ADDR_POS,
           (frame->fcf & IEEE_802154_FCF_FRAME_VER_MASK) >> IEEE_802154_FCF_FRAME_VER_POS,
           (frame->fcf & IEEE_802154_FCF_SRC_ADDR_MASK) >> IEEE_802154_FCF_SRC_ADDR_POS);
}
