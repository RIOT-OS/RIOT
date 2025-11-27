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
 * @brief       DALI peripheral lowlevel implementation
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

/* Enable DEBUG may break DALI timing ! */
#define ENABLE_DEBUG 0
#include "debug.h"

#include "ztimer.h"
#include "ztimer/periodic.h"

static void _encode_tx_frame(dali_intf_t* intf, uint8_t data)
{
    uint16_t tmp_data = 0;

    /* Clear the whole buffer */
    memset(intf->txbuf_ptr, 0, sizeof(uint8_t) * DALI_PERIPH2HOST_BUFFER_SIZE);

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
    /* START | ADDR[7..6] */
    intf->txbuf_ptr[0] = ((tmp_data & 0x000F) << 4) | 0x0F;
    intf->txbuf_ptr[1] = ((tmp_data >> 4) & 0xFF);
    intf->txbuf_ptr[2] = ((DALI_RISING_EDGE << 4) | ((tmp_data >> 12) & 0x0F));
}

int dali_peripheral_init(dali_peripheral_t* dev, const dali_conf_t* cfg, uint8_t addr)
{
    dali_intf_t* intf = &dev->intf;
    int ret;

    /* Initialize low level interface */
    ret = dali_intf_init(intf, cfg);
    if (ret != 0) {
        printf("[dali_periph]: Fail to initialize DALI intf, error:%d\n", ret);
        return 0;
    }
    dev->addr = addr;

    /* Give interface its local buffer */
    intf->txbuf_ptr = dev->tx_buf;

    puts("[dali_periph]: Node is ready");
    return 0;
}

int dali_peripheral_recv(dali_peripheral_t* dev, uint8_t* addr, uint8_t* data)
{
    dali_intf_t* intf = &dev->intf;
    /* setup reception */
    intf->rx_idx = DALI_HOST2PERIPH_XFER_BITS_SIZE;
    intf->rx_frame = 0;
    intf->prev_time = 0;
    intf->rx_state = WAIT_FOR_START;
    /* Initialize reception */
    dali_intf_setup_reception(intf);
    /* Block until an interrupt fires, starts the timeout timer
       and expires or a full frame is received */
    mutex_lock(&intf->timeout);
    /* Timer has expired or frame was received, disable interrupt */
    gpio_irq_disable(intf->cfg->rx_pin);
    /* If we received data, process it otherwise we hit a timeout */
    if (intf->rx_idx == 0 ) {
        DEBUG("[dali_periph]: frame received\n");
        /* Retrieves addr and data from frame */
        *addr = ((intf->rx_frame >> 10) & 0xFF);
        *data = ((intf->rx_frame >> 2 ) & 0xFF);
        DEBUG("[dali_periph]: addr:%x reg:%x\n", *addr, *data);
    }
    else {
        DEBUG_PUTS("[dali_periph]: error on reception");
        return -ENXIO;
    }

    return 0;
}

int dali_peripheral_reply(dali_peripheral_t* dev, uint8_t data)
{
    dali_intf_t* intf = &dev->intf;
    ztimer_sleep(ZTIMER_MSEC, 10);
    /* setup reception so 11 bits multiply by 2 to have the manchester encoding */
    intf->tx_idx = (DALI_PERIPH2HOST_XFER_BITS_SIZE * 2)-1;
    /* Initialize transmission */
    dali_intf_setup_transmission(intf);
    /* prepare reply buffer */
    _encode_tx_frame(intf, data);
    /* Start tx timer to generate the frame on TX_PIN */
    ztimer_periodic_start(&intf->timer_tx);
    /* Wait for transmission */
    mutex_lock(&intf->xfer_pending);
    /* Disable transmission */
    dali_intf_setup_reception(intf);

    return 0;
}
