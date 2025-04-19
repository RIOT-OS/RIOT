/*
 * Copyright (C) 2023 Prime Controls
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_onewire
 * @{
 *
 * @file
 * @brief       Soft 1-Wire bus driver implementation
 *
 * @author      Joshua DeWeese <jdeweese@primecontrols.com>
 *
 * @}
 */

#include "soft_onewire.h"

#include <assert.h>
#include <errno.h>

#include "irq.h"

#define ENABLE_DEBUG 0
#include "debug.h"

//TODO: use bitfield.h for tx/rx buffers?

/* bus timings as given in
 * https://www.maximintegrated.com/en/design/technical-documents/app-notes/1/126.html
 * all in microseconds
 */
 //TODO: better names?
#define T_RESET_HOLD_US     (480U)
#define T_RESET_SAMPLE_US   (70U)
#define T_RESET_RECOVERY_US (410U)
#define T_RW_START_US       (6U)
#define T_W_0_HOLD_US       (60U)
#define T_W_0_END_US        (10U)
#define T_W_1_DELAY_US      (64U)
#define T_R_WAIT_US         (9U)
#define T_R_END_US          (55U)

static void _reset_release_cb(soft_onewire_t *dev);
static void _reset_sample_cb(soft_onewire_t *dev);
static void _reset_finished_cb(soft_onewire_t *dev);
static void _read_pull_cb(soft_onewire_t *dev);
static void _read_release_cb(soft_onewire_t *dev);
static void _read_sample_cb(soft_onewire_t *dev);
static void _write_pull_cb(soft_onewire_t *dev);
static void _write_0_release_cb(soft_onewire_t *dev);
static void _write_1_release_cb(soft_onewire_t *dev);

/* drive the bus high to power any bus powered devices */
static void _bus_power(soft_onewire_t *dev)
{
#ifdef MODULE_SOFT_ONEWIRE_2PINS
    gpio_t pin = dev->params->tx_pin;
    gpio_clear(pin);
#else
    gpio_t pin = dev->params->pin;

    /* Re-initializing a GPIO pin is supposed to leave it's set/clear state
       untouched, but not all platforms can guarantee this. So we set the pin
       before and after re-initializing it. This is to avoid momentarily pulling
       the bus low. */
    gpio_set(pin);
    gpio_init(pin, GPIO_OUT);
    gpio_set(pin);
#endif
}

/* pull the bus low */
static void _bus_pull(soft_onewire_t *dev)
{
#ifdef MODULE_SOFT_ONEWIRE_2PINS
    gpio_t pin = dev->params->tx_pin;
    gpio_set(pin);
#else
    gpio_t pin = dev->params->pin;

    gpio_init(pin, GPIO_OUT);
    gpio_clear(pin);
#endif
}

/* release the bus low, allow it to idle high unless another device is holding
   it low */
static void _bus_release(soft_onewire_t *dev)
{
#ifdef MODULE_SOFT_ONEWIRE_2PINS
    gpio_t pin = dev->params->tx_pin;
    gpio_clear(pin);
#else
    gpio_t pin      = dev->params->pin;
    gpio_t pin_mode = dev->params->pin_imode;

    gpio_init(pin, pin_mode);
#endif
}

/* sample the bus - returns true if the line is high */
static bool _bus_sample(soft_onewire_t *dev)
{
#ifdef MODULE_SOFT_ONEWIRE_2PINS
    gpio_t pin = dev->params->rx_pin;
#else
    gpio_t pin = dev->params->pin;
#endif

    return gpio_read(pin);
}

/* schedule the next timer callback */
static void _schedule(soft_onewire_t *dev, soft_onewire_timer_cb_t cb,
    unsigned usec)
{
#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
    dev->timer_cb = cb;

    tim_t timer = dev->params->timer;
    const int res = timer_set(timer, 0, usec);
    if (res < 0)
    {
        /* release bus, but don't power it in-case a slave deice is trying to
           hold it low*/
        _bus_release(dev);

        /* stop ongoing process, and indicate error to waiting thread */
        dev->buf_size = -EIO;
        mutex_unlock(&dev->sync);
    }
#else
    ztimer_t *timer = &dev->timer;

    timer->callback = (ztimer_callback_t)cb;
    ztimer_set(ZTIMER_USEC, timer, usec);
#endif
}

static void _reset_release_cb(soft_onewire_t *dev)
{
    _bus_release(dev);
    _schedule(dev, &_reset_sample_cb, T_RESET_SAMPLE_US);
}

static void _reset_sample_cb(soft_onewire_t *dev)
{
    /* if the bus isn't being held low, no device is present */
    dev->buf_size = (_bus_sample(dev)) ? -ENXIO : 0;

    _schedule(dev, &_reset_finished_cb, T_RESET_RECOVERY_US);
}

static void _reset_finished_cb(soft_onewire_t *dev)
{
    _bus_power(dev);
    mutex_unlock(&dev->sync);
}

static int _reset(void *lldev)
{
    soft_onewire_t *dev = lldev;

    DEBUG("soft_onewire: resetting bus\n");

    /* begin the bus reset */
    const int irq_state = irq_disable();
    _bus_pull(dev);
    _schedule(dev, &_reset_release_cb, T_RESET_HOLD_US);
    irq_restore(irq_state);

    DEBUG("soft_onewire: waiting for reset\n");

    mutex_lock(&dev->sync);

    DEBUG("soft_onewire: reset done\n");

    return dev->buf_size;
}

static void _read_pull_cb(soft_onewire_t *dev)
{
    dev->buf_size--;

    /* if all bits have been read */
    if (dev->buf_size == 0) {
        mutex_unlock(&dev->sync);
        return;
    }

    dev->mask <<= 1;

    if (dev->mask == 0) {
        dev->mask = 1;
        dev->rx_buf++;
        *dev->rx_buf = 0;
    }

    _bus_pull(dev);
    _schedule(dev, &_read_release_cb, T_RW_START_US);
}

static void _read_release_cb(soft_onewire_t *dev)
{
    _bus_release(dev);
    _schedule(dev, &_read_sample_cb, T_R_WAIT_US);
}

static void _read_sample_cb(soft_onewire_t *dev)
{
    const bool rx_bit = _bus_sample(dev);
    *dev->rx_buf |= (rx_bit) ? dev->mask : 0;
    _schedule(dev, &_read_pull_cb, T_R_END_US);
}

static int _read(void *lldev, void *buf, size_t len)
{
    soft_onewire_t *dev = lldev;

    DEBUG("soft_onewire: begin read bits\n");

    dev->rx_buf = buf;
    dev->buf_size = len;
    dev->mask = 1;

    /* Clear first byte of the buffer, because sampling only sets bits. */
    *dev->rx_buf = 0;

    /* start the transfer */
    const int irq_state = irq_disable();
    _bus_pull(dev);
    _schedule(dev, _read_release_cb, T_RW_START_US);
    irq_restore(irq_state);

    DEBUG("soft_onewire: waiting for data\n");

    /* wait for transfer to finish */
    mutex_lock(&dev->sync);

    DEBUG("soft_onewire: got data (res = %i)\n", dev->buf_size);

    return dev->buf_size;
}

static void _write_pull_cb(soft_onewire_t *dev)
{
    dev->buf_size--;

    /* if all bits have been written */
    if (dev->buf_size == 0) {
        mutex_unlock(&dev->sync);
        return;
    }

    dev->mask <<= 1;

    if (dev->mask == 0) {
        dev->mask = 1;
        dev->tx_buf++;
    }

    const bool tx_bit = *dev->tx_buf & dev->mask;
    const unsigned release_time = (tx_bit) ? T_RW_START_US : T_W_0_HOLD_US;
    const soft_onewire_timer_cb_t release_cb = (tx_bit) ?
        _write_1_release_cb : _write_0_release_cb;

    _bus_pull(dev);
    _schedule(dev, release_cb, release_time);
}

static void _write_0_release_cb(soft_onewire_t *dev)
{
    _bus_release(dev);
    _schedule(dev, &_write_pull_cb, T_W_0_END_US);
}

static void _write_1_release_cb(soft_onewire_t *dev)
{
    _bus_release(dev);
    _schedule(dev, &_write_pull_cb, T_W_1_DELAY_US);
}

static int _write(void *lldev, const void *buf, size_t len)
{
    soft_onewire_t *dev = lldev;

    DEBUG("soft_onewire: begin write bits\n");

    dev->tx_buf = buf;
    dev->buf_size = len;
    dev->mask = 1;

    const bool tx_bit = *dev->tx_buf & dev->mask;
    const unsigned release_time = (tx_bit) ? T_RW_START_US : T_W_0_HOLD_US;
    const soft_onewire_timer_cb_t release_cb =
        (tx_bit) ? _write_1_release_cb : _write_0_release_cb;

    /* start the transfer */
    const int irq_state = irq_disable();
    _bus_pull(dev);
    _schedule(dev, release_cb, release_time);
    irq_restore(irq_state);

    DEBUG("soft_onewire: sending data\n");

    /* wait for transfer to finish */
    mutex_lock(&dev->sync);

    DEBUG("soft_onewire: data sent (res = %i)\n", dev->buf_size);

    return dev->buf_size;
}

#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
static void _timer_cb(void *arg, int channel)
{
    (void)channel;

    soft_onewire_t *dev = arg;

    dev->timer_cb(arg);
}
#endif

static int _init(void *lldev, const void *params)
{
    assert(lldev);
    assert(params);

    soft_onewire_t *dev = lldev;
    dev->params = params;

    static mutex_t locked_mutex = MUTEX_INIT_LOCKED;
    dev->sync = locked_mutex;

#ifdef MODULE_SOFT_ONEWIRE_HWTIMER
    tim_t timer = dev->params->timer;
    const int res = timer_init(timer, MHZ(1), &_timer_cb, dev);
    if (res < 0) {
        return -EIO;
    }
#else
    dev->timer.arg = dev;
#endif

#ifdef MODULE_SOFT_ONEWIRE_2PINS
    gpio_init(dev->params->rx_pin, GPIO_IN);
    gpio_init(dev->params->tx_pin, GPIO_OUT);
#endif

    _bus_power(dev);

    return 0;
}

const onewire_driver_t soft_onewire_driver = {
    .init = &_init,
    .reset = &_reset,
    .read_bits = &_read,
    .write_bits = &_write,
};
