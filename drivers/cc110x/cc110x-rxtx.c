/*
 * Copyright (C) Kaspar Schleiser <kaspar@schleiser.de>
 *               2014 Freie Universität Berlin
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
#include "cc110x-spi.h"
#include "cc110x-internal.h"
#include "cc110x-interface.h"
#include "cc110x-defines.h"

#include "periph/gpio.h"
#include "irq.h"

#include "kernel_types.h"
#include "msg.h"

#include "cpu_conf.h"
#include "cpu.h"
#include "xtimer.h"
#include "random.h"

#include "log.h"

/* WARNING: make sure the thread running cc110x has enough stack space
 * for debugging. When using gnrc, e.g., also enable debug in
 * auto_init_cc110x.c. */
#define ENABLE_DEBUG (0)
#include "debug.h"

static void _rx_start(cc110x_t *dev);
static void _rx_continue(cc110x_t *dev, void(*callback)(void*), void*arg);
static void _rx_read_data(cc110x_t *dev, void(*callback)(void*), void*arg);
static void _rx_stop(cc110x_t *dev);
static void _tx_continue(cc110x_t *dev);
static void _tx_abort(cc110x_t *dev);

static void _rx_start(cc110x_t *dev)
{
    dev->radio_state = RADIO_RX_BUSY;

    cc110x_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    pkt_buf->pos = 0;
}

static void _rx_continue(cc110x_t *dev, void(*callback)(void*), void*arg)
{
    gpio_irq_disable(dev->params.gdo2);
    if (dev->radio_state != RADIO_RX_BUSY) {
        DEBUG("%s:%s:%u _rx_continue in invalid state\n", RIOT_FILE_RELATIVE,
                __func__, __LINE__);
        _rx_stop(dev);
        return;
    }

    do {
        _rx_read_data(dev, callback, arg);
    } while (gpio_read(dev->params.gdo2));
    gpio_irq_enable(dev->params.gdo2);
}

static void _rx_read_data(cc110x_t *dev, void(*callback)(void*), void*arg)
{
    int fifo = cc110x_get_reg_robust(dev, 0xfb);

    if (fifo & 0x80) {
        DEBUG("%s:%s:%u rx overflow\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _rx_stop(dev);
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
     * leave at least one byte as per spec sheet. */
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
        int crc_ok = status[I_LQI] & CRC_OK;

        if (crc_ok) {
            LOG_DEBUG("cc110x: received packet from=%u to=%u payload "
                    "len=%u\n",
                    (unsigned)pkt_buf->packet.phy_src,
                    (unsigned)pkt_buf->packet.address,
                    pkt_buf->packet.length-3);

            /* count successful packet */
            dev->cc110x_statistic.packets_in++;

            /* let someone know that we've got a packet */
            callback(arg);
        }
        else {
            DEBUG("%s:%s:%u crc-error\n", RIOT_FILE_RELATIVE, __func__,
                    __LINE__);
            dev->cc110x_statistic.packets_in_crc_fail++;
        }

        /* we're done. */
        _rx_stop(dev);

    } else {
        left -= to_read;
        if (left > 32) {
            /* assert gdo2 when RX FIFO over threshold, deassert when below */
            cc110x_write_reg(dev, CC110X_IOCFG2, 0x0);
        } else {
            /* assert gdo2 when RX FIFO over threshold, deassert atpacket end */
            cc110x_write_reg(dev, CC110X_IOCFG2, 0x1);
            left -= 2;
            if (left > 0) {
                xtimer_usleep(left * CC110X_USEC_PER_BYTE);
            }
        }
    }
}

static void _rx_stop(cc110x_t *dev)
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

    /* did we just start sending this packet? */
    if (left == size) {
#ifdef MODULE_CC110X_HOOKS
        cc110x_hook_tx();
#endif

        /* disable CCA mode */
        cc110x_write_reg(dev, CC110X_MCSM1, 0x3);

        /* Switch to TX mode */
        cc110x_strobe(dev, CC110X_STX);
    }

    if (to_send < left) {
        /* set GDO2 to 0x2 + invert -> will assert at TX FIFO below threshold */
        cc110x_write_reg(dev, CC110X_IOCFG2, 0x02 | (0x1 << 6));
    }
    else {
        /* set GDO2 to 0x6 + invert -> will assert at packet end */
        cc110x_write_reg(dev, CC110X_IOCFG2, 0x06 | (0x1 << 6));
    }

    gpio_irq_enable(dev->params.gdo2);
}

static void _tx_abort(cc110x_t *dev)
{
    cc110x_switch_to_rx(dev);
}

void cc110x_switch_to_rx(cc110x_t *dev)
{
    DEBUG("cc110x: switching to RX.\n");

    gpio_irq_disable(dev->params.gdo2);

#ifdef MODULE_CC110X_HOOKS
    cc110x_hook_rx();
#endif

    /* flush RX fifo */
    cc110x_strobe(dev, CC110X_SIDLE);
    cc110x_strobe(dev, CC110X_SFRX);

    dev->radio_state = RADIO_RX;

    cc110x_write_reg(dev, CC110X_IOCFG2, 0x1);
    cc110x_strobe(dev, CC110X_SRX);

    gpio_irq_enable(dev->params.gdo2);
}

void cc110x_isr_handler(cc110x_t *dev, void(*callback)(void*), void*arg)
{
    switch (dev->radio_state) {
        case RADIO_RX:
            _rx_start(dev);
            /* fall through */
        case RADIO_RX_BUSY:
            _rx_continue(dev, callback, arg);
            break;
        case RADIO_TX_BUSY:
            cc110x_switch_to_rx(dev);
            _tx_continue(dev);
            break;
        default:
            DEBUG("%s:%s:%u: unhandled mode\n", RIOT_FILE_RELATIVE,
                    __func__, __LINE__);
    }
}

static int cc110x_cca(cc110x_t *dev)
{
    char regtmp;

    /* switch to idle and then RX to clear RSSI value */
    cc110x_strobe(dev, CC110X_SIDLE);
    cc110x_strobe(dev, CC110X_SRX);

    /* set CCA mode */
    cc110x_write_reg(dev, CC110X_MCSM1, (0x3 << 4) | 0x3);

    /* wait until device switched to RX */
    do {
        cc110x_readburst_reg(dev, CC110X_MARCSTATE, &regtmp, 1);
    } while (regtmp != 0x0D);

    /* set GDO2 to CCA */
    cc110x_write_reg(dev, CC110X_IOCFG2, 0x09);

    /* wait until RSSI value is stable */
    do {
        cc110x_readburst_reg(dev, CC110X_PKTSTATUS, &regtmp, 1);
    } while (! (regtmp & 0x50)); /* bit6 = CS, bit4 = CCA */

    return (regtmp & 0x10); /* CCA */
}

int cc110x_send(cc110x_t *dev, cc110x_pkt_t *packet)
{
    DEBUG("cc110x: snd pkt to %u payload_length=%u\n",
            (unsigned)packet->address, (unsigned)packet->length-3);
    uint8_t size;

    switch (dev->radio_state) {
        case RADIO_RX_BUSY:
        case RADIO_TX_BUSY:
            puts("cc110x: invalid state for sending.\n");
            return -EBUSY;
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

    unsigned retries;
    for (retries = 0; retries < CC110X_CCA_MAXTRIES; retries++) {
        xtimer_usleep(CC110X_CCA_RETRY_DELAY * (genrand_uint32() >> 22));
        if (cc110x_cca(dev)) {
            break;
        }
    }

    if(retries == CC110X_CCA_MAXTRIES) {
        DEBUG("cc110x: cca failed.\n");
        cc110x_switch_to_rx(dev);
        return -EAGAIN;
    } else {
        if (retries) {
            DEBUG("cc110x: cca ok, needed %u tries.\n", retries);
        }
    }

    /* Put CC110x in IDLE mode to flush the FIFO */
    cc110x_strobe(dev, CC110X_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc110x_strobe(dev, CC110X_SFTX);

    memcpy((char*)&dev->pkt_buf.packet, packet, size);
    dev->pkt_buf.pos = 0;

    _tx_continue(dev);

    return size;
}
