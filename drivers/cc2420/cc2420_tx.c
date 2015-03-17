/**
  * cc2420_rx.c - Implementation of transmitting cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */

#include <stdio.h>

#include "cc2420.h"
#include "cc2420_spi.h"
#include "cc2420_settings.h"
#include "cc2420_arch.h"
#include "ieee802154_frame.h"

#include "irq.h"
#include "hwtimer.h"

#include "debug.h"

static void cc2420_gen_pkt(uint8_t *buf, cc2420_packet_t *packet);
static uint8_t sequence_nr;
static bool wait_for_ack;

radio_tx_status_t cc2420_load_tx_buf(ieee802154_packet_kind_t kind,
                                     ieee802154_node_addr_t dest,
                                     bool use_long_addr,
                                     bool wants_ack,
                                     void *buf,
                                     unsigned int len)
{
    uint8_t hdr[24];

    /* FCS : frame version 0, we don't manage security,
       nor batchs of packets */
    switch (kind) {
    case PACKET_KIND_BEACON:
        hdr[0] = 0x00;
        break;
    case PACKET_KIND_DATA:
        hdr[0] = 0x01;
        break;
    case PACKET_KIND_ACK:
        hdr[0] = 0x02;
        break;
    default:
        return RADIO_TX_INVALID_PARAM;
    }

    if (wants_ack) {
        hdr[0] |= 0x20;
    }
    wait_for_ack = wants_ack;

    uint16_t src_pan = cc2420_get_pan();
    bool compress_pan = false;

    if (use_long_addr) {
        hdr[1] = 0xcc;
    } else {
        hdr[1] = 0x88;
        /* short address mode, use PAN ID compression
            for intra-PAN communication */
        if (dest.pan.id == src_pan) {
            compress_pan = true;
            hdr[0] |= 0x40;
        }
    }

    /* sequence number */
    hdr[2] = sequence_nr++;

    /* variable-length fields */
    int idx = 3;

    if (use_long_addr) {
        /* dest long addr */
        hdr[idx++] = (uint8_t)(dest.long_addr & 0xFF);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 8);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 16);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 24);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 32);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 40);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 48);
        hdr[idx++] = (uint8_t)(dest.long_addr >> 56);
        /* src long addr */
        uint64_t src_long_addr = cc2420_get_address_long();
        hdr[idx++] = (uint8_t)(src_long_addr & 0xFF);
        hdr[idx++] = (uint8_t)(src_long_addr >> 8);
        hdr[idx++] = (uint8_t)(src_long_addr >> 16);
        hdr[idx++] = (uint8_t)(src_long_addr >> 24);
        hdr[idx++] = (uint8_t)(src_long_addr >> 32);
        hdr[idx++] = (uint8_t)(src_long_addr >> 40);
        hdr[idx++] = (uint8_t)(src_long_addr >> 48);
        hdr[idx++] = (uint8_t)(src_long_addr >> 56);
    } else {
        /* dest PAN ID */
        hdr[idx++] = (uint8_t)(dest.pan.id & 0xFF);
        hdr[idx++] = (uint8_t)(dest.pan.id >> 8);
        /* dest short addr */
        hdr[idx++] = (uint8_t)(dest.pan.addr & 0xFF);
        hdr[idx++] = (uint8_t)(dest.pan.addr >> 8);
        /* src PAN ID */
        if (!compress_pan) {
            uint16_t src_pan = cc2420_get_pan();
            hdr[idx++] = (uint8_t)(src_pan & 0xFF);
            hdr[idx++] = (uint8_t)(src_pan >> 8);
        }
        /* src short addr */
        uint16_t src_addr = cc2420_get_address();
        hdr[idx++] = (uint8_t)(src_addr & 0xFF);
        hdr[idx++] = (uint8_t)(src_addr >> 8);
    }

    /* total frame size */
    uint8_t size = idx + len + 2;
    if (size > CC2420_MAX_PKT_LENGTH) {
        return RADIO_TX_PACKET_TOO_LONG;
    }

    /* flush TX buffer */
    cc2420_strobe(CC2420_STROBE_FLUSHTX);

    /* write length, header and payload to TX FIFO */
    cc2420_write_fifo(&size, 1);
    cc2420_write_fifo(hdr, idx);
    cc2420_write_fifo(buf, len);

    return RADIO_TX_OK;
}

#define CC2420_ACK_WAIT_DELAY_uS   1000
#define ACK_LENGTH  5

radio_tx_status_t cc2420_transmit_tx_buf(void)
{
    /* check if channel clear */
    if (!cc2420_channel_clear()) {
        return RADIO_TX_MEDIUM_BUSY;
    }

    /* put tranceiver in idle mode */
    cc2420_strobe(CC2420_STROBE_RFOFF);

    /* begin transmission: wait for preamble to be sent */
    unsigned int cpsr = disableIRQ();
    cc2420_strobe(CC2420_STROBE_TXON);

    int abort_count = 0;
    while (cc2420_get_sfd() == 0) {
        /* Wait for SFD signal to be set -> sync word transmitted */
        abort_count++;

        if (abort_count > CC2420_SYNC_WORD_TX_TIME) {
            /* Abort waiting. CC2420 maybe in wrong mode
               e.g. sending preambles for always */
            puts("[CC2420 TX] fatal error: could not send packet\n");
            return RADIO_TX_ERROR;
        }
    }

    restoreIRQ(cpsr);

    /* wait for packet to be sent, i.e.: SFD to go down */
    uint8_t st;
    do {
        st = cc2420_status_byte();
    } while (cc2420_get_sfd() != 0);
    cc2420_switch_to_rx();

    /* check for underflow error flag */
    if (st & CC2420_STATUS_TX_UNDERFLOW) {
        return RADIO_TX_UNDERFLOW;
    }

    /* wait for ACK only if needed */
    if (!wait_for_ack) {
        return RADIO_TX_OK;
    }

    /* delay for the peer to answer our packet */
    //TODO design a more robust method?
    hwtimer_wait(HWTIMER_TICKS(CC2420_ACK_WAIT_DELAY_uS));
    /* try to read a returning ACK packet */
    if (cc2420_get_fifop()) {
        uint8_t ackbuf[ACK_LENGTH];
        /* a packet has arrived, read the arrived data */
        cc2420_read_fifo(ackbuf, ACK_LENGTH);
        if (ackbuf[0] == 0x02  /* ack packet in buffer */
             && (ackbuf[2] == sequence_nr - 1)) /* correct sequence number */
            return RADIO_TX_OK;
    }
    return RADIO_TX_NOACK;
}

radio_tx_status_t cc2420_do_send(ieee802154_packet_kind_t kind,
                                 ieee802154_node_addr_t dest,
                                 bool use_long_addr,
                                 bool wants_ack,
                                 void *buf,
                                 unsigned int len)
{
    radio_tx_status_t st = cc2420_load_tx_buf(kind, dest,
                                              use_long_addr,
                                              wants_ack,
                                              buf, len);
    if (st != RADIO_TX_OK) {
        return st;
    }
    return cc2420_transmit_tx_buf();
}

int16_t cc2420_send(cc2420_packet_t *packet)
{
    volatile uint32_t abort_count = 0;

    /* Set missing frame information */
    packet->frame.fcf.frame_ver = 0;

    if (packet->frame.src_pan_id == packet->frame.dest_pan_id) {
        packet->frame.fcf.panid_comp = 1;
    }
    else {
        packet->frame.fcf.panid_comp = 0;
    }

    if (packet->frame.fcf.src_addr_m == 2) {
        packet->frame.src_addr[0] = (uint8_t)(cc2420_get_address() >> 8);
        packet->frame.src_addr[1] = (uint8_t)(cc2420_get_address() & 0xFF);
    }
    else if (packet->frame.fcf.src_addr_m == 3) {
        packet->frame.src_addr[0] = (uint8_t)(cc2420_get_address_long() >> 56);
        packet->frame.src_addr[1] = (uint8_t)(cc2420_get_address_long() >> 48);
        packet->frame.src_addr[2] = (uint8_t)(cc2420_get_address_long() >> 40);
        packet->frame.src_addr[3] = (uint8_t)(cc2420_get_address_long() >> 32);
        packet->frame.src_addr[4] = (uint8_t)(cc2420_get_address_long() >> 24);
        packet->frame.src_addr[5] = (uint8_t)(cc2420_get_address_long() >> 16);
        packet->frame.src_addr[6] = (uint8_t)(cc2420_get_address_long() >> 8);
        packet->frame.src_addr[7] = (uint8_t)(cc2420_get_address_long() & 0xFF);
    }

    packet->frame.src_pan_id = cc2420_get_pan();
    packet->frame.seq_nr = sequence_nr;

    sequence_nr += 1;

    /* calculate size of the package (header + payload + fcs) */
    packet->length = ieee802154_frame_get_hdr_len(&packet->frame) +
                     packet->frame.payload_len + 2;

    if (packet->length > CC2420_MAX_PKT_LENGTH) {
        return -1;
    }

    /* FCS is added in hardware */
    uint8_t pkt[packet->length - 2];

    /* generate pkt */
    cc2420_gen_pkt(pkt, packet);

    /* idle & flush tx */
    cc2420_strobe(CC2420_STROBE_RFOFF);
    cc2420_strobe(CC2420_STROBE_FLUSHTX);

    /* write length and packet to fifo */
    cc2420_write_fifo(&packet->length, 1);
    cc2420_write_fifo(pkt, packet->length - 2);

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

    while (index < packet->length - 2) {
        buf[index] = packet->frame.payload[index - offset];
        index += 1;
    }
}
