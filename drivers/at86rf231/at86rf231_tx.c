/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       TX related functionality for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "at86rf231.h"
#include "at86rf231_spi.h"
#include "hwtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _MAX_RETRIES   (100)

static int16_t at86rf231_load(at86rf231_packet_t *packet);
static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet);

static uint8_t sequence_nr;
static uint8_t wait_for_ack;

int16_t at86rf231_send(at86rf231_packet_t *packet)
{
    int16_t result;
    result = at86rf231_load(packet);
    if (result < 0) {
        return result;
    }
    at86rf231_transmit_tx_buf(NULL);
    return result;
}

static uint8_t txpkt[AT86RF231_MAX_PKT_LENGTH + 1];

netdev_802154_tx_status_t at86rf231_load_tx_buf(netdev_t *dev,
        netdev_802154_pkt_kind_t kind,
        netdev_802154_node_addr_t *dest,
        int use_long_addr,
        int wants_ack,
        netdev_hlist_t *upper_layer_hdrs,
        void *buf,
        unsigned int len)
{
    int delay;
    (void)dev;

    /* ensure the radio transceiver is up and running */
    if (!at86rf231_is_on()) {
        DEBUG("at86rf231: cannot load a packet when transceiver is off!\n");
        return NETDEV_802154_TX_STATUS_DEV_IS_OFF;
    }

    /* minimum size of frame header (FCS and SEQ are *always* mandatory!) */
    uint8_t index = 4;

    /* frame type */
    switch (kind) {
        case NETDEV_802154_PKT_KIND_BEACON:
            txpkt[1] = 0x00;
            break;
        case NETDEV_802154_PKT_KIND_DATA:
            txpkt[1] = 0x01;
            break;
        case NETDEV_802154_PKT_KIND_ACK:
            txpkt[1] = 0x02;
            break;
        default:
            return NETDEV_802154_TX_STATUS_INVALID_PARAM;
    }

    if (wants_ack) {
        txpkt[1] |= 0x20;
    }

    wait_for_ack = wants_ack;

    uint16_t src_pan = at86rf231_get_pan();
    uint8_t compress_pan = 0;

    if (use_long_addr) {
        txpkt[2] = 0xcc;
    }
    else {
        txpkt[2] = 0x88;
        if (dest->pan.id == src_pan) {
            compress_pan = 1;
            txpkt[1] |= 0x40;
        }
    }

    txpkt[3] = sequence_nr++;

    /* First 3 bytes are fixed with FCS and SEQ, resume with index = 4 */
    if (use_long_addr) {
        txpkt[index++] = (uint8_t)(dest->long_addr & 0xFF);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 8);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 16);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 24);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 32);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 40);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 48);
        txpkt[index++] = (uint8_t)(dest->long_addr >> 56);

        uint64_t src_long_addr = at86rf231_get_address_long();
        txpkt[index++] = (uint8_t)(src_long_addr & 0xFF);
        txpkt[index++] = (uint8_t)(src_long_addr >> 8);
        txpkt[index++] = (uint8_t)(src_long_addr >> 16);
        txpkt[index++] = (uint8_t)(src_long_addr >> 24);
        txpkt[index++] = (uint8_t)(src_long_addr >> 32);
        txpkt[index++] = (uint8_t)(src_long_addr >> 40);
        txpkt[index++] = (uint8_t)(src_long_addr >> 48);
        txpkt[index++] = (uint8_t)(src_long_addr >> 56);
    }
    else {
        txpkt[index++] = (uint8_t)(dest->pan.id & 0xFF);
        txpkt[index++] = (uint8_t)(dest->pan.id >> 8);

        txpkt[index++] = (uint8_t)(dest->pan.addr & 0xFF);
        txpkt[index++] = (uint8_t)(dest->pan.addr >> 8);

        if (!compress_pan) {
            txpkt[index++] = (uint8_t)(src_pan & 0xFF);
            txpkt[index++] = (uint8_t)(src_pan >> 8);
        }

        uint16_t src_addr = at86rf231_get_address();
        txpkt[index++] = (uint8_t)(src_addr & 0xFF);
        txpkt[index++] = (uint8_t)(src_addr >> 8);
    }

    /* total frame size:
     * index -> frame header
     * len   -> payload length
     * + lengths of upper layers' headers */
    size_t size = index + len;
    if (upper_layer_hdrs != NULL) {
        size += netdev_get_hlist_len(upper_layer_hdrs);
    }

    if (size > AT86RF231_MAX_PKT_LENGTH) {
        DEBUG("at86rf231: packet too long, dropped it.\n");
        return NETDEV_802154_TX_STATUS_PACKET_TOO_LONG;
    }

    /* AT86RF231 needs the PPDU size in PHR */
    txpkt[0] = size + 1;
    /* Copy upper layers' headers, if any */
    if (upper_layer_hdrs != NULL) {
        netdev_hlist_t *ptr = upper_layer_hdrs;
        do {
            uint8_t * hdr = ptr->header;
            for (int i = 0; i < ptr->header_len; i++) {
                txpkt[index++] = hdr[i];
            }
            netdev_hlist_advance(&ptr);
        } while (ptr != upper_layer_hdrs);
    }
    /* Copy payload into local TX buffer */
    uint8_t * payload = buf;
    for (int i = 0; i < len; i++) {
        txpkt[index++] = payload[i];
    }

    /* change into transmission (PLL_ON) state */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE,
                        AT86RF231_TRX_STATE__PLL_ON);
    delay = _MAX_RETRIES;
    do {
        if (!--delay) {
            DEBUG("at86rf231 : ERROR : could not enter PLL_ON mode\n");
            return NETDEV_802154_TX_STATUS_ERROR;
        }
    } while (at86rf231_current_mode() != AT86RF231_TRX_STATUS__PLL_ON);

    /* ...then go into TX_ARET_ON state if ACKnowledgement is needed */
    if (wait_for_ack) {
        at86rf231_reg_write(AT86RF231_REG__TRX_STATE,
                            AT86RF231_TRX_STATE__TX_ARET_ON);
        delay = _MAX_RETRIES;
        do {
            if (!--delay) {
                DEBUG("at86rf231 : ERROR : could not enter TX_ARET_ON mode\n");
                return NETDEV_802154_TX_STATUS_ERROR;
            }
        } while (at86rf231_current_mode() != AT86RF231_TRX_STATUS__TX_ARET_ON);
    }

    /* load transceiver buffer (FIFO) */
    at86rf231_write_fifo(txpkt, size + 1);

#if ENABLE_DEBUG
    DEBUG("Frame buffer loaded with %d bytes.\n", size);
    for (int i = 0; i < index; i++) {
        DEBUG(" %2.2x", txpkt[i]);
    }
    DEBUG("\n");
#endif

    return NETDEV_802154_TX_STATUS_OK;
}

netdev_802154_tx_status_t at86rf231_transmit_tx_buf(netdev_t *dev)
{
    (void)dev;

    /* ensure the radio transceiver is up and running */
    if (!at86rf231_is_on()) {
        DEBUG("at86rf231: cannot emit a packet when transceiver is off!\n");
        return NETDEV_802154_TX_STATUS_DEV_IS_OFF;
    }

DEBUG("Mode == %x\n", at86rf231_current_mode());
    /* radio driver state: sending */
    /* will be freed in at86rf231_rx_irq when TRX_END interrupt occurs */
    driver_state = AT_DRIVER_STATE_SENDING;

    /* Start TX */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__TX_START);
/*    gpio_set(AT86RF231_SLEEP);*/
    DEBUG("at86rf231: Started TX\n");
    /* wait for TX to actually begin */
/*    gpio_clear(AT86RF231_SLEEP);*/

    /* if no ACK is needed, the procedure is simple */
    if (!wait_for_ack) {
        DEBUG("at86rf231: Don't wait for ACK, TX done.\n");
        return NETDEV_802154_TX_STATUS_OK;
    }

    uint8_t trac_status;
    do {
        trac_status = at86rf231_reg_read(AT86RF231_REG__TRX_STATE);
        trac_status &= AT86RF231_TRX_STATE_MASK__TRAC;
    }
    while (trac_status == AT86RF231_TRX_STATE__TRAC_INVALID);

    switch (trac_status) {
        case AT86RF231_TRX_STATE__TRAC_CHANNEL_ACCESS_FAILURE:
            return NETDEV_802154_TX_STATUS_MEDIUM_BUSY;

        case AT86RF231_TRX_STATE__TRAC_NO_ACK:
            return NETDEV_802154_TX_STATUS_NOACK;

        default:
            return NETDEV_802154_TX_STATUS_OK;
    }
}

int16_t at86rf231_load(at86rf231_packet_t *packet)
{
    int delay;

    /* ensure the radio transceiver is up and running */
    if (!at86rf231_is_on()) {
        DEBUG("at86rf231: cannot load a packet when transceiver is off!\n");
        return NETDEV_802154_TX_STATUS_DEV_IS_OFF;
    }

    // Set missing frame information
    packet->frame.fcf.frame_ver = 0;

    packet->frame.src_pan_id = at86rf231_get_pan();

    if (packet->frame.src_pan_id == packet->frame.dest_pan_id) {
        packet->frame.fcf.panid_comp = 1;
    }
    else {
        packet->frame.fcf.panid_comp = 0;
    }

    if (packet->frame.fcf.src_addr_m == 2) {
        packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address() >> 8);
        packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address() & 0xFF);
    }
    else if (packet->frame.fcf.src_addr_m == 3) {
        packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address_long() >> 56);
        packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address_long() >> 48);
        packet->frame.src_addr[2] = (uint8_t)(at86rf231_get_address_long() >> 40);
        packet->frame.src_addr[3] = (uint8_t)(at86rf231_get_address_long() >> 32);
        packet->frame.src_addr[4] = (uint8_t)(at86rf231_get_address_long() >> 24);
        packet->frame.src_addr[5] = (uint8_t)(at86rf231_get_address_long() >> 16);
        packet->frame.src_addr[6] = (uint8_t)(at86rf231_get_address_long() >> 8);
        packet->frame.src_addr[7] = (uint8_t)(at86rf231_get_address_long() & 0xFF);
    }

    packet->frame.seq_nr = sequence_nr++;

    /* calculate size of the frame (payload + FCS) */
    packet->length = ieee802154_frame_get_hdr_len(&packet->frame) +
                     packet->frame.payload_len + 1;

    if (packet->length > AT86RF231_MAX_PKT_LENGTH) {
        DEBUG("at86rf231: ERROR: packet too long, dropped it.\n");
        return -1;
    }

    /* FCS is added in hardware */
    uint8_t pkt[packet->length];

    /* generate pkt */
    at86rf231_gen_pkt(pkt, packet);

    /* Go to state PLL_ON */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

    /* wait until it is on PLL_ON state */
    delay = _MAX_RETRIES;
    do {
        if (!--delay) {
            DEBUG("at86rf231 : ERROR : could not enter PLL_ON mode\n");
            break;
        }
    } while (at86rf231_current_mode() != AT86RF231_TRX_STATUS__PLL_ON);

    /* change into TX_ARET_ON state */
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__TX_ARET_ON);
    delay = _MAX_RETRIES;
    do {
        if (!--delay) {
            DEBUG("at86rf231 : ERROR : could not enter TX_ARET_ON mode\n");
            break;
        }
    } while (at86rf231_current_mode() != AT86RF231_TRX_STATUS__TX_ARET_ON);

    /* load packet into fifo */
    at86rf231_write_fifo(pkt, packet->length);
    DEBUG("at86rf231: Wrote to FIFO\n");

    return packet->length;
}

/**
 * @brief Static function to generate byte array from at86rf231 packet.
 */
static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet)
{
    uint8_t index, offset;
    index = ieee802154_frame_init(&packet->frame, &buf[1]);

    // add length for at86rf231
    buf[0] = packet->length + 1;
    index++;

    offset = index;

    while (index < packet->length) {
        buf[index] = packet->frame.payload[index - offset];
        index += 1;
    }
}
