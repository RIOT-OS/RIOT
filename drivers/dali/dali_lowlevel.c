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
 * @brief       DALI lowlevel protocol implementation
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

#define ENABLE_DEBUG 0
#include "debug.h"

#include "ztimer.h"
#include "ztimer/periodic.h"

/** Define half bit period (us) for 1200B/s */
#define DALI_HALF_BIT_PERIOD 416

#define TIMEOUT_XFER_MS 200
#define TIMEOUT_STOP_US 1666

static void _timeout_xfer(void* arg)
{
    dali_intf_t* intf = (dali_intf_t*)arg;

    mutex_unlock(&intf->timeout);
}

static void _timeout_stop(void* arg)
{
    dali_intf_t* intf = (dali_intf_t*)arg;
    /* No need to wait for timeout anymore */
    ztimer_remove(ZTIMER_MSEC, &intf->timeout_xfer);
    /* add stop bits in the frame */
    intf->rx_frame |= 0x3;
    intf->rx_idx = 0;
    /* unlock reception mutex */
    mutex_unlock(&intf->timeout);
}

static void _sample_rx(dali_intf_t* intf)
{
    /* Sample RX pin and store the bit into the right position in the frame */
    intf->rx_idx--;
    intf->rx_frame |= (gpio_read(intf->cfg->rx_pin) << (intf->rx_idx));
}

static void _wait_for_transition(dali_intf_t *intf, ztimer_now_t diff_time)
{
    if ((diff_time > 370) && (diff_time < 460)) {
        /* if previous interrupt was also a half bit, then we received
            a whole bit */
        if (intf->rx_state == HALF_BIT) {
            _sample_rx(intf);
            intf->rx_state = SAMPLING;
        }
        else {
            intf->rx_state = HALF_BIT;
        }
    }
    /* new transition around a bit period +/-10% (1/1200)*/
    else if ((diff_time > 740) & (diff_time < 920)) {
        _sample_rx(intf);
    }
    else {
        /* We're doom if we get here */
        assert(0);
    }
}

static int _tx_xfer(void* arg)
{
    dali_intf_t* intf = (dali_intf_t*)arg;
    const dali_conf_t *cfg = intf->cfg;
    uint8_t buf_idx = (intf->tx_idx / 8);
    gpio_write(cfg->tx_pin, (intf->txbuf_ptr[buf_idx] >> (intf->tx_idx%8)) & 0x1);

    if (intf->tx_idx == 0) {
        /* Transfer is done, release mutex */
        ztimer_periodic_stop(&intf->timer_tx);
        mutex_unlock(&intf->xfer_pending);
        return true;
    }
    intf->tx_idx--;
    return false;
}

static void _rx_int(void* arg)
{
    dali_intf_t* intf = (dali_intf_t*)arg;
    ztimer_now_t cur_time = ztimer_now(ZTIMER_USEC);
    ztimer_now_t diff_time = cur_time - intf->prev_time;
    /* On first RX interrupt for DALI peripheral, need to start
       the transfer timeout */
#ifdef MODULE_DALI_PERIPHERAL
    if (!ztimer_is_set(ZTIMER_MSEC, &intf->timeout_xfer)) {
        ztimer_set(ZTIMER_MSEC, &intf->timeout_xfer, TIMEOUT_XFER_MS);
    }
#endif

    /* First transition, prepare reception of a new frame */
    switch (intf->rx_state) {
        case WAIT_FOR_START:
            intf->rx_state = HALF_BIT;
            break;
        case SAMPLING:  /* Fall-through */
        case HALF_BIT:
            _wait_for_transition(intf, diff_time);
            if (intf->rx_idx == 2) {
                /* There will be one more transition before the bus IDLE
                   so wait for it before waiting for our stop bits */
                if (!gpio_read(intf->cfg->rx_pin)) {
                    intf->rx_state = WAIT_FOR_IDLE;
                }
                else {
                    intf->rx_state = WAIT_FOR_STOP;
                    ztimer_set(ZTIMER_USEC, &intf->timeout_stop, TIMEOUT_STOP_US);
                }
            }
            break;
        case WAIT_FOR_IDLE:
            ztimer_set(ZTIMER_USEC, &intf->timeout_stop, TIMEOUT_STOP_US);
            intf->rx_state = WAIT_FOR_STOP;
            break;
        case WAIT_FOR_STOP:
            ztimer_remove(ZTIMER_USEC, &intf->timeout_stop);
            break;
        default:
            intf->rx_state = WAIT_FOR_START;
            intf->rx_idx = DALI_PERIPH2HOST_XFER_BITS_SIZE;
    }

    intf->prev_time = cur_time;
}

void dali_intf_setup_reception(dali_intf_t* dev)
{
    const dali_conf_t *cfg = dev->cfg;

    /* Disable TX */
    gpio_init(cfg->tx_pin, GPIO_IN);
    /* Enable RX interruption */
    gpio_irq_enable(cfg->rx_pin);
}

void dali_intf_setup_transmission(dali_intf_t* dev)
{
    const dali_conf_t *cfg = dev->cfg;

    /* Disable RX interruption */
    gpio_irq_disable(cfg->rx_pin);
    /* Enable TX */
    gpio_init(cfg->tx_pin, GPIO_OUT);
    gpio_set(cfg->tx_pin);
}

int dali_intf_init(dali_intf_t* dev, const dali_conf_t* cfg)
{
    puts("[dali]: Initialize DALI interface");
    dev->cfg = cfg;
    /* Check if RX/TX pins are defined */
    if (!gpio_is_valid(cfg->rx_pin) || !gpio_is_valid(cfg->rx_pin)) {
        puts("[dali]: at least one GPIO is misconfigured");
        return -EINVAL;
    }
    /* Initialize GPIO interrupt */
    gpio_init_int(cfg->rx_pin, GPIO_IN, GPIO_BOTH, _rx_int, (void*)dev);

    gpio_irq_disable(cfg->rx_pin);
    /* Initialize mutex & lock mutex*/
    mutex_init(&dev->xfer_pending);
    mutex_lock(&dev->xfer_pending);
    mutex_init(&dev->timeout);
    mutex_lock(&dev->timeout);
    /* Initialize ztimer used for transmission */
    ztimer_periodic_init(ZTIMER_USEC, &dev->timer_tx, _tx_xfer, (void*)dev, DALI_HALF_BIT_PERIOD);

    dev->timeout_xfer.callback = _timeout_xfer;
    dev->timeout_xfer.arg = (void*)dev;
    dev->timeout_stop.callback = _timeout_stop;
    dev->timeout_stop.arg = (void*)dev;
    puts("[dali]: DALI interface ready");
    return 0;
}
