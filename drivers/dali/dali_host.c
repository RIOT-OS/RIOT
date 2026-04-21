/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dali
 * @{
 *
 * @file
 * @brief       DALI host lowlevel implementation
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"

#include "dali.h"
#include "dali_params.h"

#include "log.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "ztimer.h"
#include "ztimer/periodic.h"

#define TX_XFER_HALF_BITS_SIZE (2 * 19)

#define TIMEOUT_XFER_MS 50
#define TIMEOUT_STOP_US 1666
/** Define half bit period (us) for 1200B/s */
#define DALI_HALF_BIT_PERIOD 416

/* Transmit first ->->->->->->->-> Transmit last */
/* START[2] -> ADDR[16] -> DATA[16] -> STOP[4] */
static void _encode_tx_frame(dali_intf_t* intf, uint8_t addr, uint8_t data)
{
    uint16_t tmp_addr = 0;
    uint16_t tmp_data = 0;

    /* Clear the whole buffer */
    memset(intf->txbuf_ptr, 0, sizeof(uint8_t) * DALI_HOST2PERIPH_BUFFER_SIZE);

    for (unsigned i = 0; i<8; i++) {
        if (addr & 0x01) {
            /* 0b01 represents a rising edge */
            tmp_addr |= ((DALI_RISING_EDGE) << (2*i));
        }
        else {
            tmp_addr |= ((DALI_FALLING_EDGE) << (2*i));
        }
        addr = addr >> 1;
    }

    for (unsigned i = 0; i<8; i++) {
        if (data & 0x01) {
            tmp_data |= ((DALI_RISING_EDGE) << (2*i));
        }
        else {
            tmp_data |= ((DALI_FALLING_EDGE) << (2*i));
        }
        data = data >> 1;
    }
    /* prepare TX buffer, note that a bit of real data is represented as
       two bits of manchester encoded data (0) -> 0b10 / (1) -> 0b01 */
    /* ADDR[1..0] | STOP[1..0] */
    /* ADDR[5..2] */
    /* DATA[1..0] | ADDR[7..6] */
    /* DATA[5..2] */
    /* START | DATA[7..6] */
    intf->txbuf_ptr[0] = ((tmp_data & 0x000F) << 4) | 0x0F;
    intf->txbuf_ptr[1] = ((tmp_data >> 4) & 0xFF);
    intf->txbuf_ptr[2] = (((tmp_addr & 0x000F) << 4)| ((tmp_data >> 12) & 0x0F));
    intf->txbuf_ptr[3] = ((tmp_addr >> 4 ) & 0xFF);
    intf->txbuf_ptr[4] = ((DALI_RISING_EDGE << 4) | ((tmp_addr >> 12) & 0x0F));

    /*TODO: removeme*/
    DEBUG("txframe:%02x%02x%02x%02x%02x\n", intf->txbuf_ptr[4], intf->txbuf_ptr[3], intf->txbuf_ptr[2], intf->txbuf_ptr[1], intf->txbuf_ptr[0]);
}

static void _dali_host_send(dali_host_t* dev, uint8_t addr, uint8_t cmd)
{
    dali_intf_t* intf = &dev->intf;
    /* setup transmission */
    dali_intf_setup_transmission(intf);
    /* Encode data to manchester */
    _encode_tx_frame(intf, addr, cmd);
    /* reset tx index */
    intf->tx_idx = TX_XFER_HALF_BITS_SIZE - 1;

    /* Start tx timer to generate the frame on TX_PIN */
    ztimer_periodic_start(&intf->timer_tx);
    /* Wait for transmission */
    mutex_lock(&intf->xfer_pending);
    /* Let the bus IDLE */
    gpio_set(intf->cfg->tx_pin);
}

static int _dali_host_recv(dali_host_t* dev, uint8_t *cmd)
{
    dali_intf_t* intf = &dev->intf;
    /* setup reception */
    intf->rx_idx = DALI_PERIPH2HOST_XFER_BITS_SIZE;
    intf->rx_frame = 0;
    intf->prev_time = 0;
    intf->rx_state = WAIT_FOR_START;
    /* Wait for answer if any */
    ztimer_set(ZTIMER_MSEC, &intf->timeout_xfer, TIMEOUT_XFER_MS);
    dali_intf_setup_reception(intf);
    /* Block until timeout or whole frame is received */
    mutex_lock(&intf->timeout);

    /* If we received data, process it otherwise we hit a timeout */
    if (intf->rx_idx == 0 ) {
        /* check if START and STOP bits are correctly set */
        if ((intf->rx_frame & 0x400) && (intf->rx_frame & 0x03)) {
            /* get the actual data */
            *cmd = (intf->rx_frame & 0x03FC) >> 2;
        }
        else {
            DEBUG("[dali_host]: decoding error\n");
            DEBUG("[dali_host]: rx_frame:%"PRIu32"\n", intf->rx_frame);
            return -EINVAL;
        }
    }
    else {
        gpio_irq_disable(intf->cfg->rx_pin);
        return -ENXIO;
    }

    return 0;
}

int dali_host_init(dali_host_t* dev, const dali_conf_t* cfg)
{
    int ret;
    dali_intf_t* intf = &dev->intf;

    /* Initialize low level interface */
    ret = dali_intf_init(intf, cfg);
    if (ret != 0) {
        printf("[dali_host]: Fail to initialize DALI intf, error:%d\n", ret);
        return 0;
    }
    /* Give interface its local buffer */
    intf->txbuf_ptr = dev->tx_buf;

    puts("[dali_host]: Host is ready");
    return 0;
}

int dali_host_xfer(dali_host_t* dev, uint8_t addr, uint8_t cmd, uint8_t* data)
{
    int ret = 0;

    _dali_host_send(dev, addr, cmd);
    /* Skip reception sequence if no pointer given */
    if (data != NULL)
    {
        ret = _dali_host_recv(dev, data);
        if (ret != 0) {
            return ret;
        }
    }
    return ret;
}
