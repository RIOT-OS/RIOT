/**
 * implementation for the IEEE 802.15.4 frame format
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    ieee802154_frame.c
 * @brief   IEEE 802.14.4 framing operations
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "ieee802154_frame.h"

#define ENABLE_DEBUG    (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define IEEE_802154_FCS_POLY    (0x8408)  /* x^16 + x^12 + x^5 + 1 for LSB first */

uint8_t ieee802154_hdr_ptr;
uint8_t ieee802154_payload_ptr;
uint16_t ieee802154_payload_len;

uint8_t ieee802154_frame_init(ieee802154_frame_t *frame, uint8_t *buf)
{
    /* Frame Control Field - 802.15.4 - 2006 - 7.2.1.1  */
    uint8_t index = 0;

    buf[index] = (((frame->fcf.frame_type)      & 0x07) |
                  ((frame->fcf.sec_enb    << 3) & 0x08) |
                  ((frame->fcf.frame_pend << 4) & 0x10) |
                  ((frame->fcf.ack_req    << 5) & 0x20) |
                  ((frame->fcf.panid_comp << 6) & 0x40));
    index++;

    buf[index] = (((frame->fcf.dest_addr_m << 2) & 0x0c) |
                  ((frame->fcf.frame_ver   << 4) & 0x30) |
                  ((frame->fcf.src_addr_m  << 6) & 0xc0));

    index++;

    /* Sequence Number - 802.15.4 - 2006 - 7.2.1.2 */
    buf[index] = frame->seq_nr;
    index++;

    /* Destination PAN Identifier - 802.15.4 - 2006 - 7.2.1.3 */
    if (frame->fcf.dest_addr_m == 0x02 || frame->fcf.dest_addr_m == 0x03) {
        buf[index + 1] = ((frame->dest_pan_id >> 8) & 0xff);
        buf[index] = (frame->dest_pan_id & 0xff);
    }

    index += 2;

    /* Destination Address - 802.15.4 - 2006 - 7.2.1.4 */
    if (frame->fcf.dest_addr_m == 0x02) {
        buf[index]     = frame->dest_addr[1];
        buf[index + 1] = frame->dest_addr[0];
        index += 2;
    }
    else if (frame->fcf.dest_addr_m == 0x03) {
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
    if (!(frame->fcf.panid_comp & 0x01)) {
        if (frame->fcf.src_addr_m == 0x02 || frame->fcf.src_addr_m == 0x03) {
            buf[index + 1]  = ((frame->src_pan_id >> 8) & 0xff);
            buf[index]      = (frame->src_pan_id & 0xff);
            index += 2;
        }
    }

    /* Source Address field - 802.15.4 - 2006 - 7.2.1.6 */
    if (frame->fcf.src_addr_m == 0x02) {
        buf[index]     = frame->src_addr[1];
        buf[index + 1] = frame->src_addr[0];
        index += 2;
    }
    else if (frame->fcf.src_addr_m == 0x03) {
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

#ifdef DEBUG_ENABLED
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
    uint8_t len = 0;

    if (frame->fcf.dest_addr_m == 0x02) {
        len += 2;
    }
    else if (frame->fcf.dest_addr_m == 0x03) {
        len += 8;
    }

    if (frame->fcf.src_addr_m == 0x02) {
        len += 2;
    }
    else if (frame->fcf.src_addr_m == 0x03) {
        len += 8;
    }

    if ((frame->fcf.dest_addr_m == 0x02) || (frame->fcf.dest_addr_m == 0x03)) {
        len += 2;
    }

    if ((frame->fcf.src_addr_m == 0x02) || (frame->fcf.src_addr_m == 0x03)) {
        len += 2;
    }

    /* if src pan id == dest pan id set compression bit */
    if (frame->src_pan_id == frame->dest_pan_id) {
        frame->fcf.panid_comp = 1;
        len -= 2;
    }

    /* (DPID + DAD + SPID + SAD) + (FCF + DSN) */
    return (len + 3);
}

uint8_t ieee802154_frame_read(uint8_t *buf, ieee802154_frame_t *frame,
                              uint8_t len)
{
    uint8_t index = 0;
    uint8_t hdrlen;
    frame->fcf.frame_type = (buf[index]) & 0x07;
    frame->fcf.sec_enb = (buf[index] >> 3) & 0x01;
    frame->fcf.frame_pend = (buf[index] >> 4) & 0x01;
    frame->fcf.ack_req = (buf[index] >> 5) & 0x01;
    frame->fcf.panid_comp = (buf[index] >> 6) & 0x01;

    index++;

    frame->fcf.dest_addr_m = (buf[index] >> 2) & 0x03;
    frame->fcf.frame_ver = (buf[index] >> 4) & 0x03;
    frame->fcf.src_addr_m = (buf[index] >> 6) & 0x03;

    index++;

    frame->seq_nr = buf[index];

    index++;

    if(frame->fcf.dest_addr_m != 0)
    {
        frame->dest_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];
        index += 2;
    }


    switch (frame->fcf.dest_addr_m) {
        case (0): {
            DEBUG("fcf.dest_addr_m: pan identifier/address fields empty\n");
            break;
        }

        case (2): {
            /* read address in little-endian order */
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index + 1];
            index += 2;
            break;
        }

        case (3): {
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
    }

    if (!(frame->fcf.panid_comp == 1)) {
        if(frame->fcf.src_addr_m != 0)
        {
            frame->src_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];
            index += 2;
        }
        }
    }

    switch (frame->fcf.src_addr_m) {
        case (0): {
            DEBUG("fcf.src_addr_m: pan identifier/address fields empty\n");
            break;
        }

        case (2): {
            /* read address in little-endian order */
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index + 1];
            index += 2;
            break;
        }

        case (3): {
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
    }

    frame->payload = (buf + index);
    hdrlen = index;
    frame->payload_len = (len - hdrlen - IEEE_802154_FCS_LEN);
    return hdrlen;
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
    printf("frame type: %02x\n"
           "security enabled: %02x\n"
           "frame pending: %02x\n"
           "ack requested: %02x\n"
           "pan id compression: %02x\n"
           "destination address mode: %02x\n"
           "frame version: %02x\n"
           "source address mode: %02x\n",
           frame->fcf.frame_type,
           frame->fcf.sec_enb,
           frame->fcf.frame_pend,
           frame->fcf.ack_req,
           frame->fcf.panid_comp,
           frame->fcf.dest_addr_m,
           frame->fcf.frame_ver,
           frame->fcf.src_addr_m);
}
