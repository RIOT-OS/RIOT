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
    gpio_irq_disable(dev->params.int_pin);

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
        gpio_irq_enable(dev->params.int_pin);
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

    DEBUG("%s: fifo=%d, left=%d, to_read=%d, pos=%d\n",
    		__func__, fifo, left, to_read, pkt_buf->pos);

    if (to_read) {
        cc1200_readburst_reg(dev, CC1200_RXFIFO,
                ((char*)&(pkt_buf->packet))+(pkt_buf->pos), to_read);
#if ENABLE_DEBUG
        uint8_t i;
        DEBUG("%s: received: ", __func__);
        uint8_t* b = (uint8_t*)&pkt_buf->packet;
		for(i=pkt_buf->pos; i<pkt_buf->pos + to_read; i++){
			DEBUG("0x%02x ", b[i]);
		}
		DEBUG("\n");
#endif

        pkt_buf->pos += to_read;
        DEBUG("%s: pos=%d\n", __func__, pkt_buf->pos);
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
            DEBUG("%s:%s:%u crc-error\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
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

    gpio_irq_disable(dev->params.int_pin);
    cc1200_write_reg(dev, CC1200_IOCFG2, CC1200_CFG_RXFIFO_THR);
    do {
        _rx_read_data(dev, callback, arg);
    }
    while (gpio_read(dev->params.int_pin));
}

static void _rx_start(cc1200_t *dev)
{
    dev->radio_state = RADIO_RX_BUSY;

    cc1200_pkt_buf_t *pkt_buf = &dev->pkt_buf;
    pkt_buf->pos = 0;

    gpio_irq_disable(dev->params.int_pin);
    cc1200_write_reg(dev, CC1200_IOCFG2, CC1200_CFG_RXFIFO_THR);

    /* If we are late, go into resceive immediatly */
    if(gpio_read(dev->params.int_pin)){
        //DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _rx_continue(dev, dev->isr_cb, dev->isr_cb_arg);
        return;
    }
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_irq_enable(dev->params.int_pin);
}

static void _tx_abort(cc1200_t *dev)
{
    cc1200_switch_to_rx(dev);
}

static void _tx_continue(cc1200_t *dev)
{
    DEBUG("%s:%s:%u\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    gpio_irq_disable(dev->params.int_pin);

    cc1200_pkt_t *pkt = &dev->pkt_buf.packet;
#if ENABLE_DEBUG
    DEBUG("%s:%s:%u printing package:\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
    for (int i = 0; i < pkt->length + 1; i++)
    {
        DEBUG("0x%x ", *(((char *)pkt) + i));
    }
    DEBUG("\n");
#endif

    int size = pkt->length + 1;

    int left = size - dev->pkt_buf.pos;

    if (!left)
    {
        dev->cc1200_statistic.raw_packets_out++;

        LOG_DEBUG("cc1200: packet successfully sent.\n");
        int tx_bytes = cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);

        while(tx_bytes > 0){
            DEBUG("%s: %d bytes to send...\n", __func__, tx_bytes);
            xtimer_usleep(50);
            tx_bytes = cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);
        }

        //xtimer_spin(xtimer_ticks_from_usec(300));
        cc1200_switch_to_rx(dev);
        return;
    }

    int fifo = CC1200_TX_FIFO_SIZE - cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);

    int status = cc1200_strobe(dev, CC1200_SNOP);
    if((status & 0x70) == 0x70){
        DEBUG("%s:%s:%u fifo over/underflow!?\n", RIOT_FILE_RELATIVE, __func__, __LINE__);
        _tx_abort(dev);
        return;
    }

    int to_send = left > fifo ? fifo : left;


    DEBUG("%s: left=%d, to_send=%d, fifo=%d, pos=%d\n",
        __func__, left, to_send, fifo, dev->pkt_buf.pos);

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
        //xtimer_usleep(200);
    }

    if (to_send < left)
    {
        /* set GDO2 to 0x2 -> will deassert at TX FIFO below threshold */
        cc1200_write_reg(dev, CC1200_IOCFG2, CC1200_CFG_TXFIFO_THR);
        gpio_irq_enable(dev->params.int_pin);
    }
    else
	{
        /* set GDO2 to 0x6 -> pin 2 will deassert at end of TX packet */
        cc1200_write_reg(dev, CC1200_IOCFG2, CC1200_CFG_PKT_SYNC_RXTX);
        gpio_irq_enable(dev->params.int_pin);
    }
}

void cc1200_isr_handler(cc1200_t *dev, void(*callback)(void*), void*arg)
{
    uint8_t rxbytes;

    switch (dev->radio_state) {
        case RADIO_RX:
            rxbytes = cc1200_read_reg(dev, CC1200_NUM_RXBYTES);
            //DEBUG("radio rx\n");
            if (gpio_read(dev->params.int_pin) | (rxbytes > 0)) {
                //DEBUG("cc1200_isr_handler((): starting RX\n");
                dev->isr_cb = callback;
                dev->isr_cb_arg = arg; 
                _rx_start(dev);

                /*
                 * Don't know, why this waiting is necessary, but the driver
                 * blocks without this. Alternatively, the delay can also be
                 * located in  _netdev_cc1200_isr() or _isr(), but this here
                 * is more specific. Also, the delay can be placed before or
                 * after, but not in _rx_start()...
                 */
                xtimer_spin(xtimer_ticks_from_usec(800));
            }
            else {
                DEBUG("cc1200_isr_handler((): isr handled too slow or falling edge trigger\n");
                _rx_abort(dev);
            }
            break;
        case RADIO_RX_BUSY:
            //DEBUG("radio rx busy\n");
            _rx_continue(dev, callback, arg);
            break;
        case RADIO_TX_BUSY:
            //DEBUG("radio tx busy\n");
            if (!gpio_read(dev->params.int_pin)) {
                _tx_continue(dev);
            }
            else {
                //DEBUG("cc1200_isr_handler() RADIO_TX_BUSY + GDO2\n");
            }
            break;
        default:
            DEBUG("%s:%s:%u: unhandled mode\n", RIOT_FILE_RELATIVE,
                    __func__, __LINE__);
            break;
    }
}

int cc1200_send(cc1200_t *dev, const struct iovec *vector, unsigned count)
{
    DEBUG("%s:%u\n", __func__, __LINE__);

    switch (dev->radio_state) {
        case RADIO_RX_BUSY:
            DEBUG("%s: rx busy...\n", __func__);
            return -EAGAIN;
        case RADIO_TX_BUSY:
            DEBUG("%s: tx busy...\n", __func__);
            //return -EAGAIN;
            {
                int tx_bytes = cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);

                while(tx_bytes > 0){
                    DEBUG("%s: waiting for transmission to be finished, %d bytes left\n",
                        __func__, tx_bytes);
                    thread_yield();
                    xtimer_spin(xtimer_ticks_from_usec(50));
                    tx_bytes = cc1200_get_reg_robust(dev, CC1200_NUM_TXBYTES);
                }

                /*
                 * need to wait a little before sending next frame;
                 * don't know why...
                 *
                 * TODO: check in detail...
                 */
                xtimer_usleep(500);
            }
    }

    gpio_irq_disable(dev->params.int_pin);
    dev->radio_state = RADIO_TX_BUSY;

    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    //size = packet->length + 1;
    uint8_t size = 0;
    for(uint8_t i = 0; i<count; i++) size += vector[i].iov_len;

    DEBUG("%s: size=%d\n", __func__, size);

    if (size > (CC1200_PACKET_LENGTH+1) ) {
        DEBUG("%s:%s:%u trying to send oversized packet\n",
                RIOT_FILE_RELATIVE, __func__, __LINE__);
        return -ENOSPC;
    }

#ifdef MODULE_CC1200_HOOKS
    cc1200_hook_tx();
#endif

    cc1200_write_reg(dev, CC1200_IOCFG2, CC1200_CFG_TXFIFO_THR);

    /* Put CC1200 in IDLE mode to flush the FIFO */
    cc1200_strobe(dev, CC1200_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc1200_strobe(dev, CC1200_SFTX);
    
    /* Flush device pkt_buf */
    memset(&dev->pkt_buf, 0, sizeof(dev->pkt_buf));

    /* Set packet length field */
    dev->pkt_buf.packet.length = size;

    DEBUG("%s:%u SIZE: %u\n", __func__, __LINE__, size);
    int pos = 1; // length field has been set by hand
    for(uint8_t i = 0; i< count; i++){
        unsigned int pkt_len = vector[i].iov_len;

#if ENABLE_DEBUG
        DEBUG("%s:%u Package to send: \n", __func__, __LINE__);
        for(uint8_t j = 0; j < pkt_len; j++){
            DEBUG("0x%x ", *(((char*) vector[i].iov_base)+j));
        }
        DEBUG("\n");
#endif

        /* Copying contents from the iovector into CC1200 own framebuffer */
        memcpy((char*)&dev->pkt_buf.packet+pos, vector[i].iov_base, pkt_len);
        pos += pkt_len;
    }
    dev->pkt_buf.pos = 0;

    DEBUG("cc1200: snd pkt to %u payload_length=%u\n",
            (unsigned)dev->pkt_buf.packet.address, 
            (unsigned)dev->pkt_buf.packet.length);
    DEBUG("\n");

    _tx_continue(dev);

    /*
     * need to wait a little before returning;
     * don't know why...
     *
     * TODO: check in detail...
     */
    xtimer_usleep(5000);
    return size;
}
