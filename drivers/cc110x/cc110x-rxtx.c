/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 * @file
 * @brief       Functions for packet reception and transmission on cc110x devices
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cc110x.h"
#include "cc110x/cc110x-spi.h"
#include "cc110x/cc110x-internal.h"
#include "cc110x/cc110x-interface.h"
#include "cc110x/cc110x-defines.h"

#include "periph/gpio.h"
#include "irq.h"

#include "kernel_types.h"
#include "msg.h"

#include "cpu_conf.h"
#include "cpu.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _rx_abort(cc110x_t *dev)
{
    gpio_irq_disable(dev->params.gdo2);

    cc110x_strobe(dev, CC110X_SIDLE);    /* Switch to IDLE (should already be)... */
    cc110x_strobe(dev, CC110X_SFRX);     /* ...for flushing the RX FIFO */

    cc110x_switch_to_rx(dev);
}

static void _rx_start(cc110x_t *dev)
{
    dev->radio_state = RADIO_RX_BUSY;

    cc110x_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    pkt_buf->pos = 0;

    gpio_irq_disable(dev->params.gdo2);
    cc110x_write_reg(dev, CC110X_IOCFG2, 0x01);
    gpio_irq_enable(dev->params.gdo2);
}

static void _rx_read_data(cc110x_t *dev, void(*callback)(void*), void*arg)
{
    int fifo = cc110x_get_reg_robust(dev, 0xfb);

    if (fifo & 0x80) {
        DEBUG("%s:%s:%u rx overflow\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _rx_abort(dev);
        return;
    }

    if (!fifo) {
        gpio_irq_enable(dev->params.gdo2);
        return;
    }

    cc110x_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    if (!pkt_buf->pos) {
        pkt_buf->pos = 1;
        pkt_buf->packet.length = cc110x_read_reg(dev, CC110X_RXFIFO);

        /* Possible packet received, RX -> IDLE (0.1 us) */
        dev->cc110x_statistic.packets_in++;
    }

    int left = pkt_buf->packet.length+1 - pkt_buf->pos;

    /* if the fifo doesn't contain the rest of the packet,
     * leav at least one byte as per spec sheet. */
    int to_read = (fifo < left) ? (fifo-1) : fifo;
    if (to_read > left) {
        to_read = left;
    }

    if (to_read) {
        cc110x_readburst_reg(dev, CC110X_RXFIFO,
                ((char *)&pkt_buf->packet)+pkt_buf->pos, to_read);
        pkt_buf->pos += to_read;
    }

    if (to_read == left) {
        uint8_t status[2];
        /* full packet received. */
        /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
        cc110x_readburst_reg(dev, CC110X_RXFIFO, (char *)status, 2);

        /* Store RSSI value of packet */
        pkt_buf->rssi = status[I_RSSI];

        /* Bit 0-6 of LQI indicates the link quality (LQI) */
        pkt_buf->lqi = status[I_LQI] & LQI_EST;

        /* MSB of LQI is the CRC_OK bit */
        int crc_ok = (status[I_LQI] & CRC_OK) >> 7;

        if (crc_ok) {
                    LOG_DEBUG("cc110x: received packet from=%u to=%u payload "
                            "len=%u\n",
                    (unsigned)pkt_buf->packet.phy_src,
                    (unsigned)pkt_buf->packet.address,
                    pkt_buf->packet.length-3);
            /* let someone know that we've got a packet */
            callback(arg);

            cc110x_switch_to_rx(dev);
        }
        else {
            DEBUG("%s:%s:%u crc-error\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
            dev->cc110x_statistic.packets_in_crc_fail++;
            _rx_abort(dev);
        }
    }
}

static void _rx_continue(cc110x_t *dev, void(*callback)(void*), void*arg)
{

    if (dev->radio_state != RADIO_RX_BUSY) {
        DEBUG("%s:%s:%u _rx_continue in invalid state\n", RIOT_FILE_RELATIVE,
                __func__, __LINE__);
        _rx_abort(dev);
        return;
    }

    gpio_irq_disable(dev->params.gdo2);

    do {
        _rx_read_data(dev, callback, arg);
    }
    while (gpio_read(dev->params.gdo2));
}

static void _tx_abort(cc110x_t *dev)
{
    cc110x_switch_to_rx(dev);
}

static void _tx_continue(cc110x_t *dev)
{
    gpio_irq_disable(dev->params.gdo2);

    cc110x_pkt_t *pkt = &dev->pkt_buf.packet;
    int size = pkt->length + 1;
    int left = size - dev->pkt_buf.pos;

    if (!left) {
        dev->cc110x_statistic.raw_packets_out++;

        LOG_DEBUG("cc110x: packet successfully sent.\n");

        cc110x_switch_to_rx(dev);
        return;
    }

    int fifo = 64 - cc110x_get_reg_robust(dev, 0xfa);

    if (fifo & 0x80) {
        DEBUG("%s:%s:%u tx underflow!\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _tx_abort(dev);
        return;
    }

    if (!fifo) {
        DEBUG("%s:%s:%u fifo full!?\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _tx_abort(dev);
        return;
    }

    int to_send = left > fifo ? fifo : left;

    /* Write packet into TX FIFO */
    cc110x_writeburst_reg(dev, CC110X_TXFIFO, ((char *)pkt)+dev->pkt_buf.pos, to_send);
    dev->pkt_buf.pos += to_send;

    if (left == size) {
        /* Switch to TX mode */
        cc110x_strobe(dev, CC110X_STX);
    }

    if (to_send < left) {
        /* set GDO2 to 0x2 -> will deassert at TX FIFO below threshold */
        gpio_irq_enable(dev->params.gdo2);
        cc110x_write_reg(dev, CC110X_IOCFG2, 0x02);
    }
    else {
        /* set GDO2 to 0x6 -> will deassert at packet end */
        cc110x_write_reg(dev, CC110X_IOCFG2, 0x06);
        gpio_irq_enable(dev->params.gdo2);
    }
}

void cc110x_isr_handler(cc110x_t *dev, void(*callback)(void*), void*arg)
{
    switch (dev->radio_state) {
        case RADIO_RX:
            if (gpio_read(dev->params.gdo2)) {
                _rx_start(dev);
            }
            else {
                DEBUG("cc110x_isr_handler((): isr handled too slow?\n");
                _rx_abort(dev);
            }
            break;
        case RADIO_RX_BUSY:
            _rx_continue(dev, callback, arg);
            break;
        case RADIO_TX_BUSY:
            if (!gpio_read(dev->params.gdo2)) {
                _tx_continue(dev);
            }
            else {
                DEBUG("cc110x_isr_handler() RADIO_TX_BUSY + GDO2\n");
            }
            break;
        default:
            DEBUG("%s:%s:%u: unhandled mode\n", RIOT_FILE_RELATIVE,
                    __func__, __LINE__);
    }
}

int cc110x_send(cc110x_t *dev, cc110x_pkt_t *packet)
{
    DEBUG("cc110x: snd pkt to %u payload_length=%u\n",
            (unsigned)packet->address, (unsigned)packet->length-3);
    uint8_t size;

    switch (dev->radio_state) {
        case RADIO_RX_BUSY:
        case RADIO_TX_BUSY:
            DEBUG("cc110x: invalid state for sending: %s\n",
                    cc110x_state_to_text(dev->radio_state));
            return -EAGAIN;
    }

    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    size = packet->length + 1;

    if (size > CC110X_PACKET_LENGTH) {
        DEBUG("%s:%s:%u trying to send oversized packet\n",
                RIOT_FILE_RELATIVE, __func__, __LINE__);
        return -ENOSPC;
    }

    /* set source address */
    packet->phy_src = dev->radio_address;

    /* Disable RX interrupt */
    gpio_irq_disable(dev->params.gdo2);
    dev->radio_state = RADIO_TX_BUSY;

#ifdef MODULE_CC110X_HOOKS
    cc110x_hook_tx();
#endif

    cc110x_write_reg(dev, CC110X_IOCFG2, 0x02);

    /* Put CC110x in IDLE mode to flush the FIFO */
    cc110x_strobe(dev, CC110X_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc110x_strobe(dev, CC110X_SFTX);

    memcpy((char*)&dev->pkt_buf.packet, packet, size);
    dev->pkt_buf.pos = 0;

    _tx_continue(dev);

    return size;
}
