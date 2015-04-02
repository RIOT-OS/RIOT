/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file        kw2xrf_tx.c
 * @brief       Transmit functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 */
#include <stdio.h>
#include <string.h>

#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_internal.h"
#include "ieee802154_frame.h"

#include "irq.h"
#include "hwtimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#define MKW2XDRF_ACK_WAIT_DELAY_uS   1000
#define ACK_LENGTH  5

static void kw2xrf_gen_pkt(uint8_t *buf, kw2xrf_packet_t *packet);
static uint8_t sequence_nr;
static bool wait_for_ack;

netdev_802154_tx_status_t kw2xrf_load_tx_buf(netdev_t *dev,
        netdev_802154_pkt_kind_t kind,
        netdev_802154_node_addr_t *dest,
        int use_long_addr,
        int wants_ack,
        netdev_hlist_t *upper_layer_hdrs,
        void *buf,
        unsigned int len)
{
    
    netdev_hlist_t *ulhdrs_ptr = upper_layer_hdrs;
    
    /* Use seperate name for hdr, with an shifted index for more intuitive addressing. */
    uint8_t pkt[MKW2XDRF_MAX_PKT_LENGTH - 1];
    uint8_t *hdr = pkt + 1;
        
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    /* FCS : frame version 0, we don't manage security,
       nor batchs of packets */
    switch (kind) {
        case NETDEV_802154_PKT_KIND_BEACON:
            hdr[0] = 0x00;
            break;

        case NETDEV_802154_PKT_KIND_DATA:
            hdr[0] = 0x01;
            break;

        case NETDEV_802154_PKT_KIND_ACK:
            hdr[0] = 0x02;
            break;

        default:
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
    }

    if (wants_ack) {
        hdr[0] |= 0x20;
    }

    wait_for_ack = wants_ack;

    uint16_t src_pan = kw2xrf_get_pan();
    bool compress_pan = false;

    if (use_long_addr) {
        hdr[1] = 0xcc;
    }
    else {
        hdr[1] = 0x88;

        /* short address mode, use PAN ID compression
            for intra-PAN communication */
        if (dest->pan.id == src_pan) {
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
        hdr[idx++] = (uint8_t)(dest->long_addr & 0xFF);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 8);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 16);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 24);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 32);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 40);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 48);
        hdr[idx++] = (uint8_t)(dest->long_addr >> 56);
        /* src long addr */
        uint64_t src_long_addr = kw2xrf_get_address_long();
        hdr[idx++] = (uint8_t)(src_long_addr & 0xFF);
        hdr[idx++] = (uint8_t)(src_long_addr >> 8);
        hdr[idx++] = (uint8_t)(src_long_addr >> 16);
        hdr[idx++] = (uint8_t)(src_long_addr >> 24);
        hdr[idx++] = (uint8_t)(src_long_addr >> 32);
        hdr[idx++] = (uint8_t)(src_long_addr >> 40);
        hdr[idx++] = (uint8_t)(src_long_addr >> 48);
        hdr[idx++] = (uint8_t)(src_long_addr >> 56);
    }
    else {
        /* dest PAN ID */
        hdr[idx++] = (uint8_t)(dest->pan.id & 0xFF);
        hdr[idx++] = (uint8_t)(dest->pan.id >> 8);
        /* dest short addr */
        hdr[idx++] = (uint8_t)(dest->pan.addr & 0xFF);
        hdr[idx++] = (uint8_t)(dest->pan.addr >> 8);

        /* src PAN ID */
        if (!compress_pan) {
            hdr[idx++] = (uint8_t)(src_pan & 0xFF);
            hdr[idx++] = (uint8_t)(src_pan >> 8);
        }

        /* src short addr */
        uint16_t src_addr = kw2xrf_get_address();
        hdr[idx++] = (uint8_t)(src_addr & 0xFF);
        hdr[idx++] = (uint8_t)(src_addr >> 8);
    }

    /* Prepend upper layer headers  */
    if (upper_layer_hdrs) {
        do {
            memcpy(&(pkt[idx + 1]), ulhdrs_ptr->header, ulhdrs_ptr->header_len);
            idx += ulhdrs_ptr->header_len;
            netdev_hlist_advance(&ulhdrs_ptr);
        } while (ulhdrs_ptr != upper_layer_hdrs);
    }

    /* total frame size, extra 2 for FCS, 1 for size in  pkt[0] */
    uint8_t size = idx + len + 3;
    if (size > MKW2XDRF_MAX_PKT_LENGTH) {
        return NETDEV_802154_TX_STATUS_PACKET_TOO_LONG;
    }
    pkt[0] = size;
    
    /* generate pkt, header data is already in pkt, header data was written above to hdr[] array. */
    for (uint8_t i = 0; i < len; i++) {
        pkt[i + idx + 1] = ((uint8_t *)buf)[i];
    }

    if (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE)) {
        /* abort any ongoing sequence */
        DEBUG("tx: abort SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));

        while (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
    }

    /* write complete packet at once, because of efficiency and the later use of dma */
    kw2xrf_write_fifo(pkt, size - 2);

    return NETDEV_802154_TX_STATUS_OK;
}

netdev_802154_tx_status_t kw2xrf_transmit_tx_buf(netdev_t *dev)
{
    /* check if channel clear */
    if (!(kw2xrf_channel_clear(dev) == 0)) {
        return NETDEV_802154_TX_STATUS_MEDIUM_BUSY;
    }

    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ);
    /* programm TR sequence */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(2));

    while (!(kw2xrf_read_dreg(MKW2XDM_IRQSTS1) & MKW2XDM_IRQSTS1_TXIRQ));

    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));
    kw2xrf_switch_to_rx();

    /* wait for ACK only if needed */
    if (!wait_for_ack) {
        return NETDEV_802154_TX_STATUS_OK;
    }

    /* delay for the peer to answer our packet */
    //TODO design a more robust method?
    hwtimer_wait(HWTIMER_TICKS(MKW2XDRF_ACK_WAIT_DELAY_uS));

    /* wait for packet to be send */
    while (!(kw2xrf_read_dreg(MKW2XDM_IRQSTS1) & MKW2XDM_IRQSTS1_TXIRQ));

    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ);

    /*TODO check seqnum */
    /* return RADIO_TX_OK; */
    return NETDEV_802154_TX_STATUS_NOACK;
}

/* TODO: add global modem cfg_struct for addresses and settings */
int16_t kw2xrf_send(kw2xrf_packet_t *packet)
{
    /* - FCS + one octet for FRAME_LEN */
    uint8_t pkt[MKW2XDRF_MAX_PKT_LENGTH - 1];

    /* Set missing frame information */
    packet->frame.fcf.frame_ver = 0;

    if (packet->frame.src_pan_id == packet->frame.dest_pan_id) {
        packet->frame.fcf.panid_comp = 1;
    }
    else {
        packet->frame.fcf.panid_comp = 0;
    }

    if (packet->frame.fcf.src_addr_m == 2) {
	uint16_t src_addr = kw2xrf_get_address();
        packet->frame.src_addr[0] = (uint8_t)(src_addr >> 8);
        packet->frame.src_addr[1] = (uint8_t)(src_addr & 0xFF);
    }
    else if (packet->frame.fcf.src_addr_m == 3) {
	uint64_t src_addr = kw2xrf_get_address_long();
        packet->frame.src_addr[0] = (uint8_t)(src_addr >> 56);
        packet->frame.src_addr[1] = (uint8_t)(src_addr >> 48);
        packet->frame.src_addr[2] = (uint8_t)(src_addr >> 40);
        packet->frame.src_addr[3] = (uint8_t)(src_addr >> 32);
        packet->frame.src_addr[4] = (uint8_t)(src_addr >> 24);
        packet->frame.src_addr[5] = (uint8_t)(src_addr >> 16);
        packet->frame.src_addr[6] = (uint8_t)(src_addr >> 8);
        packet->frame.src_addr[7] = (uint8_t)(src_addr & 0xFF);
    }

    packet->frame.src_pan_id = kw2xrf_get_pan();
    packet->frame.seq_nr = sequence_nr;

    sequence_nr += 1;

    /* calculate size of the package (header + payload + fcs) */
    packet->length = ieee802154_frame_get_hdr_len(&packet->frame) +
                     packet->frame.payload_len + 2;

    if (packet->length > MKW2XDRF_MAX_PKT_LENGTH) {
        return -1;
    }

    if (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE)) {
        /* abort any ongoing sequence */
        DEBUG("tx: abort SEQ_STATE: %x\n", kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
        kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));
    	while (kw2xrf_read_dreg(MKW2XDM_SEQ_STATE));
    }

    /* generate pkt */
    pkt[0] = packet->length;
    kw2xrf_gen_pkt((pkt + 1), packet);
    kw2xrf_write_fifo(pkt, packet->length - 1);

    kw2xrf_write_dreg(MKW2XDM_IRQSTS1, MKW2XDM_IRQSTS1_TXIRQ);
    /* programm TR sequence */
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(2));
    while (!(kw2xrf_read_dreg(MKW2XDM_IRQSTS1) & MKW2XDM_IRQSTS1_TXIRQ));
    kw2xrf_write_dreg(MKW2XDM_PHY_CTRL1, MKW2XDM_PHY_CTRL1_XCVSEQ(0));
    
    kw2xrf_switch_to_rx();

    return packet->length;
}

static void kw2xrf_gen_pkt(uint8_t *buf, kw2xrf_packet_t *packet)
{
    uint8_t index, offset;
    index = ieee802154_frame_init(&packet->frame, buf);
    offset = index;

    while (index < packet->length - 2) {
        buf[index] = packet->frame.payload[index - offset];
        index += 1;
    }
}
/** @} */
