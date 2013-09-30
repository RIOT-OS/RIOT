/**
  * cc2420_rx.c - Implementation of transmitting cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */

#include <stdio.h>

#include <cc2420.h>
#include <cc2420_spi.h>
#include <cc2420_settings.h>
#include <cc2420_arch.h>
#include <ieee802154_frame.h>

#include <irq.h>

#define ENABLE_DEBUG    (0)
#include "debug.h"

static void cc2420_gen_pkt(uint8_t *buf, cc2420_packet_t *packet);

static uint8_t sequenz_nr;

int16_t cc2420_send(cc2420_packet_t *packet)
{
    volatile uint32_t abort_count = 0;

    /* Set missing frame information */
    packet->frame.fcf.frame_ver = 0;
    if(packet->frame.src_pan_id == packet->frame.dest_pan_id) {
        packet->frame.fcf.panid_comp = 1;
    } else {
        packet->frame.fcf.panid_comp = 0;
    }

    if(packet->frame.fcf.src_addr_m == 2) {
        packet->frame.src_addr[1] = (uint8_t)(cc2420_get_address() >> 8);
        packet->frame.src_addr[0] = (uint8_t)(cc2420_get_address() & 0xFF);
    } else if (packet->frame.fcf.src_addr_m == 3) {
        packet->frame.src_addr[7] = (uint8_t)(cc2420_get_address_long() >> 56);
        packet->frame.src_addr[6] = (uint8_t)(cc2420_get_address_long() >> 48);
        packet->frame.src_addr[5] = (uint8_t)(cc2420_get_address_long() >> 40);
        packet->frame.src_addr[4] = (uint8_t)(cc2420_get_address_long() >> 32);
        packet->frame.src_addr[3] = (uint8_t)(cc2420_get_address_long() >> 24);
        packet->frame.src_addr[2] = (uint8_t)(cc2420_get_address_long() >> 16);
        packet->frame.src_addr[1] = (uint8_t)(cc2420_get_address_long() >> 8);
        packet->frame.src_addr[0] = (uint8_t)(cc2420_get_address_long() & 0xFF);
    }
    packet->frame.src_pan_id = cc2420_get_pan();
    packet->frame.seq_nr = sequenz_nr;

    sequenz_nr += 1;

    /* calculate size of the package (header + payload + fcs) */
    packet->length = ieee802154_frame_get_hdr_len(&packet->frame) +
                     packet->frame.payload_len + 2;

    if(packet->length > CC2420_MAX_PKT_LENGTH) {
        return -1;
    }
    /* FCS is added in hardware */
    uint8_t pkt[packet->length-2];

    /* generate pkt */
    cc2420_gen_pkt(pkt, packet);

    /* idle & flush tx */
    cc2420_strobe(CC2420_STROBE_RFOFF);
    cc2420_strobe(CC2420_STROBE_FLUSHTX);

    /* write length and packet to fifo */
    cc2420_write_fifo(&packet->length, 1);
    cc2420_write_fifo(pkt, packet->length-2);

    unsigned int cpsr = disableIRQ();
    cc2420_strobe(CC2420_STROBE_TXON);

    // Wait for SFD to be set -> sync word transmitted
    while (cc2420_get_sfd() == 0) {
           abort_count++;
           if (abort_count > CC2420_SYNC_WORD_TX_TIME) {
               // Abort waiting. CC2420 maybe in wrong mode
               // e.g. sending preambles for always
               puts("[CC2420 TX] fatal error\n");
               /* TODO: error handling */
               packet->length = 0;
               break;
         }
    }
    DEBUG("SEQ: %u\n", packet->frame.seq_nr);
    restoreIRQ(cpsr);

    /* wait for packet to be send */
    while (cc2420_get_sfd() != 0);

    cc2420_switch_to_rx();
    return packet->length;
}

/**
 * @brief Static function to generate byte array from cc2420 packet.
 *
 */

static void cc2420_gen_pkt(uint8_t *buf, cc2420_packet_t *packet)
{
    uint8_t index, offset;
    index = ieee802154_frame_init(&packet->frame, buf);
    offset = index;
    while(index < packet->length-2) {
        buf[index] = packet->frame.payload[index-offset];
        index += 1;
    }
    cc2420_swap_fcf_bytes(buf);
}
