/**
 * implementation for the IEEE 802.15.4 frame format 
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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


uint8_t ieee802154_hdr_ptr;
uint8_t ieee802154_payload_ptr;
uint16_t ieee802154_payload_len;

uint8_t init_802154_frame(ieee802154_frame_t *frame, uint8_t *buf)
{
    /* Frame Control Field - 802.15.4 - 2006 - 7.2.1.1  */
    uint8_t index = 0;

    buf[index] = ((frame->fcf.frame_type << 5) |
                  (frame->fcf.sec_enb << 4) |
                  (frame->fcf.frame_pend << 3) |
                  (frame->fcf.ack_req << 2) |
                  (frame->fcf.panid_comp << 1));
    index++;
    buf[index] = ((frame->fcf.dest_addr_m << 4) |
                  (frame->fcf.frame_ver << 2) |
                  (frame->fcf.src_addr_m));
    index++;

    /* Sequence Number - 802.15.4 - 2006 - 7.2.1.2 */
    buf[index] = frame->seq_nr;
    index++;

    /* Destination PAN Identifier - 802.15.4 - 2006 - 7.2.1.3 */
    if(frame->fcf.dest_addr_m == 0x02 || frame->fcf.dest_addr_m == 0x03) {
        buf[index] = ((frame->dest_pan_id >> 8) & 0xff);
        buf[index + 1] = (frame->dest_pan_id & 0xff);
    }

    index += 2;

    /* Destination Address - 802.15.4 - 2006 - 7.2.1.4 */
    if(frame->fcf.dest_addr_m == 0x02) {
        buf[index]     = frame->dest_addr[0];
        buf[index + 1] = frame->dest_addr[1];
        index += 2;
    }
    else if(frame->fcf.dest_addr_m == 0x03) {
        buf[index]     = frame->dest_addr[0];
        buf[index + 1] = frame->dest_addr[1];
        buf[index + 2] = frame->dest_addr[2];
        buf[index + 3] = frame->dest_addr[3];
        buf[index + 4] = frame->dest_addr[4];
        buf[index + 5] = frame->dest_addr[5];
        buf[index + 6] = frame->dest_addr[6];
        buf[index + 7] = frame->dest_addr[7];
        index += 8;
    }

    /* Source PAN Identifier - 802.15.4 - 2006 - 7.2.1.5 */
    if(!(frame->fcf.panid_comp & 0x01)) {
        if(frame->fcf.src_addr_m == 0x02 || frame->fcf.src_addr_m == 0x03) {
            buf[index]     = ((frame->src_pan_id >> 8) & 0xff);
            buf[index + 1] = (frame->src_pan_id & 0xff);
            index += 2;
        }
    }

    /* Source Address field - 802.15.4 - 2006 - 7.2.1.6 */
    if(frame->fcf.src_addr_m == 0x02) {
        buf[index]     = frame->src_addr[0];
        buf[index + 1] = frame->src_addr[1];
        index += 2;
    }
    else if(frame->fcf.src_addr_m == 0x03) {
        buf[index]     = frame->src_addr[0];
        buf[index + 1] = frame->src_addr[1];
        buf[index + 2] = frame->src_addr[2];
        buf[index + 3] = frame->src_addr[3];
        buf[index + 4] = frame->src_addr[4];
        buf[index + 5] = frame->src_addr[5];
        buf[index + 6] = frame->src_addr[6];
        buf[index + 7] = frame->src_addr[7];
        index += 8;
    }

    return index;
}

/**     2      1      2     VAR    2      VAR
  * -------------------------------------------
  * |  FCF  | DSN | DPID  | DAD | SPID  | SAD |
  * -------------------------------------------
  */
uint8_t get_802154_hdr_len(ieee802154_frame_t *frame)
{
    uint8_t len = 0;

    if(frame->fcf.dest_addr_m == 0x02) {
        len += 2;
    }
    else if(frame->fcf.dest_addr_m == 0x03) {
        len += 8;
    }

    if(frame->fcf.src_addr_m == 0x02) {
        len += 2;
    }
    else if(frame->fcf.src_addr_m == 0x03) {
        len += 8;
    }

    if((frame->fcf.dest_addr_m == 0x02) || (frame->fcf.dest_addr_m == 0x03)) {
        len += 2;
    }

    if((frame->fcf.src_addr_m == 0x02) || (frame->fcf.src_addr_m == 0x03)) {
        len += 2;
    }

    /* if src pan id == dest pan id set compression bit */
    if(frame->src_pan_id == frame->dest_pan_id) {
        frame->fcf.panid_comp = 1;
        len -= 2;
    }

    /* (DPID + DAD + SPID + SAD) + (FCF + DSN) */
    return (len + 3);
}

uint8_t read_802154_frame(uint8_t *buf, ieee802154_frame_t *frame, uint8_t len)
{
    uint8_t index = 0;
    uint8_t hdrlen;

    frame->fcf.frame_type = (buf[index] >> 5) & 0x07;
    frame->fcf.sec_enb = (buf[index] >> 4) & 0x01;
    frame->fcf.frame_pend = (buf[index] >> 3) & 0x01;
    frame->fcf.ack_req = (buf[index] >> 2) & 0x01;
    frame->fcf.panid_comp = (buf[index] >> 1) & 0x01;

    index++;

    frame->fcf.dest_addr_m = (buf[index] >> 4) & 0x03;
    frame->fcf.frame_ver = (buf[index] >> 2) & 0x03;
    frame->fcf.src_addr_m = buf[index] & 0x03;

    //print_802154_fcf_frame(frame);

    index++;

    frame->seq_nr = buf[index];

    index++;

    frame->dest_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];

    index += 2;

    switch(frame->fcf.dest_addr_m) {
        case(0): {
            printf("fcf.dest_addr_m: pan identifier/address fields empty\n");
            break;
        }

        case(2): {
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index + 1];
            index += 2;
            break;
        }

        case(3): {
            frame->dest_addr[0] = buf[index];
            frame->dest_addr[1] = buf[index + 1];
            frame->dest_addr[2] = buf[index + 2];
            frame->dest_addr[3] = buf[index + 3];
            frame->dest_addr[4] = buf[index + 4];
            frame->dest_addr[5] = buf[index + 5];
            frame->dest_addr[6] = buf[index + 6];
            frame->dest_addr[7] = buf[index + 7];
            index += 8;
            break;
        }
    }

    if(!(frame->fcf.panid_comp == 1)) {
        frame->src_pan_id = (((uint16_t)buf[index]) << 8) | buf[index + 1];
        index += 2;
    }

    switch(frame->fcf.src_addr_m) {
        case(0): {
            printf("fcf.src_addr_m: pan identifier/address fields empty\n");
            break;
        }

        case(2): {
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index + 1];
            index += 2;
            break;
        }

        case(3): {
            frame->src_addr[0] = buf[index];
            frame->src_addr[1] = buf[index + 1];
            frame->src_addr[2] = buf[index + 2];
            frame->src_addr[3] = buf[index + 3];
            frame->src_addr[4] = buf[index + 4];
            frame->src_addr[5] = buf[index + 5];
            frame->src_addr[6] = buf[index + 6];
            frame->src_addr[7] = buf[index + 7];
            index += 8;
            break;
        }
    }

    frame->payload = (buf + index);
    hdrlen = index;
    frame->payload_len = (len - hdrlen);

    return hdrlen;
}

void print_802154_fcf_frame(ieee802154_frame_t *frame)
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
