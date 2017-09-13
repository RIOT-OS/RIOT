/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc1200
 * @{
 * @file
 * @brief       Functions for packet reception and transmission on cc1200 devices
 *
 * @author      Anon Mall <mail@anonmall.com>
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "cc1200.h"
#include "cc1200-spi.h"
#include "cc1200-internal.h"
#include "cc1200-interface.h"
#include "cc1200-defines.h"

#include "periph/gpio.h"
#include "irq.h"

#include "kernel_types.h"
#include "msg.h"
#include "xtimer.h"

#include "cpu_conf.h"
#include "cpu.h"

#include "log.h"
#include "led.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _rx_abort(cc1200_t *dev)
{
    gpio_irq_disable(dev->params.gdo2);

    cc1200_strobe(dev, CC1200_SIDLE);    /* Switch to IDLE (should already be)... */
    cc1200_strobe(dev, CC1200_SFRX);     /* ...for flushing the RX FIFO */

    cc1200_switch_to_rx(dev);
}



static void _rx_read_data(cc1200_t *dev, void(*callback)(void*), void*arg)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);

    uint8_t overflow = cc1200_strobe(dev, CC1200_SNOP);
    if((overflow & 0x70) == 0x60){
        DEBUG("%s:%s:%u rx overflow\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _rx_abort(dev);
        return;
    }


    uint8_t fifo = cc1200_read_reg(dev, CC1200_NUM_RXBYTES);

    if (!fifo) {
        DEBUG("%s:%s:%u rx fifo is empty\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        gpio_irq_enable(dev->params.gdo2);
        return;
    }

    cc1200_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    if (!pkt_buf->pos) {
        pkt_buf->pos = 1;

        pkt_buf->packet.length = cc1200_read_reg(dev, CC1200_RXFIFO);

        /* Possible packet received, RX -> IDLE (0.1 us) */
        dev->cc1200_statistic.packets_in++;
    }

    int left = pkt_buf->packet.length+1 - pkt_buf->pos;

    /* if the fifo doesn't contain the rest of the packet,
     * leav at least one byte as per spec sheet. */
    int to_read = (fifo < left) ? (fifo-1) : fifo;
    if (to_read > left) {
        to_read = left;
    }

    if (to_read) {
        cc1200_readburst_reg(dev, CC1200_RXFIFO,
                ((char*)&(pkt_buf->packet))+(pkt_buf->pos), to_read);
        pkt_buf->pos += to_read;
    }

    if (to_read == left) {
        uint8_t status[2];
        /* full packet received. */
        /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
        cc1200_readburst_reg(dev, CC1200_RXFIFO, (char *)status, 2);
        DEBUG("%s:%u status0: 0x%x\n", __func__, __LINE__, status[0]);
        DEBUG("%s:%u status1: 0x%x\n", __func__, __LINE__, status[1]);


        /* Store RSSI value of packet */
        pkt_buf->rssi = status[I_RSSI];

        /* Bit 0-6 of LQI indicates the link quality (LQI) */
        pkt_buf->lqi = status[I_LQI] & LQI_EST;

        /* MSB of LQI is the CRC_OK bit */
        
#ifndef CC1200_CHECK_CRC
        /*We are using the CRC autoflush config, so when we get here, the CRC check already suceeded */
        int crc_ok = 1;
#else
        int crc_ok = (status[I_LQI] & CRC_OK) >> 7;
#endif
        if (crc_ok)
        {
            DEBUG("cc1200: received packet from=%u to=%u payload "
                  "len=%u\n",
                  (unsigned)pkt_buf->packet.phy_src,
                  (unsigned)pkt_buf->packet.address,
                  pkt_buf->packet.length - 3);

            /* let someone know that we've got a packet */
            callback(arg);

            cc1200_switch_to_rx(dev);
        }
        else
        {
            //DEBUG("%s:%s:%u crc-error\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
            dev->cc1200_statistic.packets_in_crc_fail++;
            _rx_abort(dev);
        }
    }
}

static void _rx_continue(cc1200_t *dev, void(*callback)(void*), void*arg)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);


    if (dev->radio_state != RADIO_RX_BUSY) {
        DEBUG("%s:%s:%u _rx_continue in invalid state\n", RIOT_FILE_RELATIVE,
                __func__, __LINE__);
        _rx_abort(dev);
        return;
    }

    gpio_irq_disable(dev->params.gdo2);
    cc1200_write_reg(dev, CC1200_IOCFG2, 0x01);
    do {
        _rx_read_data(dev, callback, arg);
    }
    while (gpio_read(dev->params.gdo2));
}

static void _rx_start(cc1200_t *dev)
{
    dev->radio_state = RADIO_RX_BUSY;

    cc1200_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    pkt_buf->pos = 0;

    gpio_irq_disable(dev->params.gdo2);
    cc1200_write_reg(dev, CC1200_IOCFG2, 0x01);

    /* If we are late, go into resceive immediatly */
    if(gpio_read(dev->params.gdo2)){
        DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _rx_continue(dev, dev->isr_cb, dev->isr_cb_arg);
    }
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_irq_enable(dev->params.gdo2);
}

static void _tx_abort(cc1200_t *dev)
{
    cc1200_switch_to_rx(dev);
}

static void _tx_continue(cc1200_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_irq_disable(dev->params.gdo2);

    cc1200_pkt_t *pkt = &dev->pkt_buf.packet;
    DEBUG("%s:%s:%u printing package:\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    for (int i = 0; i < pkt->length + 1; i++)
    {
        DEBUG("0x%x ", *(((char *)pkt) + i));
    }
    DEBUG("\n");

    int size = pkt->length + 1;

    int left = size - dev->pkt_buf.pos;

    if (!left)
    {
        dev->cc1200_statistic.raw_packets_out++;

        LOG_DEBUG("cc1200: packet successfully sent.\n");

        cc1200_switch_to_rx(dev);
        return;
    }

    int fifo = CC1200_PACKET_LENGTH - cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);

    int status = cc1200_strobe(dev, CC1200_SNOP);
    if((status & 0x70) == 0x70){
        DEBUG("%s:%s:%u fifo over/underflow!?\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _tx_abort(dev);
        return;
    }
    int to_send = left > fifo ? fifo : left;


    /* Flush TX  and calibrate Crystal */
    cc1200_strobe(dev, CC1200_SFTX);
    cc1200_strobe(dev, CC1200_SFSTXON);

    /* Write packet into TX FIFO */
    cc1200_writeburst_reg(dev, CC1200_TXFIFO, ((char *)pkt) + dev->pkt_buf.pos, to_send);
    dev->pkt_buf.pos += to_send;

    if (left == size)
    {
        /* Switch to TX mode */
        cc1200_strobe(dev, CC1200_STX);
        xtimer_usleep(200);
    }

    if (to_send < left)
    {
        /* set GDO2 to 0x2 -> will deassert at TX FIFO below threshold */
        cc1200_write_reg(dev, CC1200_IOCFG2, 0x02);
        gpio_irq_enable(dev->params.gdo2);
    }
    else
        {
            /* set GDO2 to 0x6 -> will deassert at packet end */
        cc1200_write_reg(dev, CC1200_IOCFG2, 0x06);
        gpio_irq_enable(dev->params.gdo2);
    }
}

void cc1200_isr_handler(cc1200_t *dev, void(*callback)(void*), void*arg)
{
uint8_t rxbytes = cc1200_read_reg(dev, CC1200_NUM_RXBYTES);

    switch (dev->radio_state) {
        case RADIO_RX:
            DEBUG("radio rx\n");
            if (gpio_read(dev->params.gdo2) | (rxbytes > 0)) {
                DEBUG("cc1200_isr_handler((): starting RX\n");
                dev->isr_cb = callback;
                dev->isr_cb_arg = arg; 
                _rx_start(dev);
            }
            else {
                DEBUG("cc1200_isr_handler((): isr handled too slow or falling edge trigger\n");
                _rx_abort(dev);
            }
            break;
        case RADIO_RX_BUSY:
            DEBUG("radio rx busy\n");
            _rx_continue(dev, callback, arg);
            break;
        case RADIO_TX_BUSY:
            DEBUG("radio tx busy\n");
            if (!gpio_read(dev->params.gdo2)) {
                _tx_continue(dev);
            }
            else {
                DEBUG("cc1200_isr_handler() RADIO_TX_BUSY + GDO2\n");
            }
            break;
        default:
            DEBUG("%s:%s:%u: unhandled mode\n", RIOT_FILE_RELATIVE,
                    __func__, __LINE__);
            break;
    }
}

int cc1200_send(cc1200_t *dev, cc1200_pkt_t *packet)
{
    DEBUG("%s:%u\n", __func__, __LINE__);
    DEBUG("cc1200: snd pkt to %u payload_length=%u\n",
            (unsigned)packet->address, (unsigned)packet->length);
    for(int i = 0; i< packet->length+1; i++)DEBUG("0x%x ", *(((char* )packet)+i));
    DEBUG("\n");

    uint8_t size;
    switch (dev->radio_state) {
        case RADIO_RX_BUSY:
            DEBUG("%s:%u\n", __func__, __LINE__);
        case RADIO_TX_BUSY:
            DEBUG("%s:%u\n", __func__, __LINE__);
            return -EAGAIN;
    }

    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    size = packet->length + 1;

    if (size > CC1200_PACKET_LENGTH) {
        DEBUG("%s:%s:%u trying to send oversized packet\n",
                RIOT_FILE_RELATIVE, __func__, __LINE__);
        return -ENOSPC;
    }

    /* Disable RX interrupt */
    gpio_irq_disable(dev->params.gdo2);
    dev->radio_state = RADIO_TX_BUSY;

#ifdef MODULE_CC1200_HOOKS
    cc1200_hook_tx();
#endif

    cc1200_write_reg(dev, CC1200_IOCFG2, 0x02);

    /* Put CC110x in IDLE mode to flush the FIFO */
    cc1200_strobe(dev, CC1200_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc1200_strobe(dev, CC1200_SFTX);

    memcpy((char*)&dev->pkt_buf.packet, packet, size);
    dev->pkt_buf.pos = 0;

    _tx_continue(dev);


    return size;
}
