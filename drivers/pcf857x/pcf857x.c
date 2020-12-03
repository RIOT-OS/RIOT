/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_pcf857x
 * @brief       Device driver for Texas Instruments PCF857X I2C I/O expanders
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#include <string.h>
#include <stdlib.h>

#include "pcf857x.h"

#include "irq.h"
#include "log.h"
#include "thread.h"
#include "xtimer.h"

#if MODULE_PCF857X_IRQ
#include "event/thread.h"
#endif

#define ENABLE_DEBUG    (0)
#include "debug.h"

#if ENABLE_DEBUG

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[pcf857x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#define ERROR_DEV(f, d, ...) \
        LOG_ERROR("[pcf857x] %s i2c dev=%d addr=%02x: " f "\n", \
                  __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

#define EXEC_RET(f) \
    do { \
        int _r; \
        if ((_r = f) != PCF857X_OK) { \
            DEBUG("[pcf857x] %s: error code %d\n", __func__, _r); \
            return _r; \
        } \
    } while (0)

#define EXEC(f) \
    do { \
        int _r; \
        if ((_r = f) != PCF857X_OK) { \
            DEBUG("[pcf857x] %s: error code %d\n", __func__, _r); \
            return; \
        } \
    } while (0)

/** Forward declaration of functions for internal use */

static int _read(const pcf857x_t *dev, pcf857x_data_t *data);
static int _write(const pcf857x_t *dev, pcf857x_data_t data);

#if MODULE_PCF857X_IRQ

/* interrutp service routine for IRQs */
static void _irq_isr(void *arg);

/* declaration of IRQ handler function */
static void _irq_handler(event_t *event);

/* internal update function */
static void _update_state(pcf857x_t* dev);

#endif /* MODULE_PCF857X_IRQ */

int pcf857x_init(pcf857x_t *dev, const pcf857x_params_t *params)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(params != NULL);
    assert(params->exp < PCF857X_EXP_MAX);
#if MODULE_PCF857X_IRQ
    assert(gpio_is_valid(params->int_pin));
#endif

    DEBUG_DEV("params=%p", dev, params);

    /* init device data structure */
    dev->params = *params;

    switch (params->exp) {
#ifdef MODULE_PCF8574
        /**< PCF8574 8 bit I/O expander used */
        case PCF857X_EXP_PCF8574: dev->pin_num = PCF8574_GPIO_PIN_NUM;
                                  dev->params.addr += PCF8574_BASE_ADDR;
                                  break;
#endif
#ifdef MODULE_PCF8574A
        /**< PCF8574A 8 bit I/O expander */
        case PCF857X_EXP_PCF8574A: dev->pin_num = PCF8574A_GPIO_PIN_NUM;
                                   dev->params.addr += PCF8574A_BASE_ADDR;
                                   break;
#endif
#ifdef MODULE_PCF8575
        /**< PCF8575 16 bit I/O expander */
        case PCF857X_EXP_PCF8575: dev->pin_num = PCF8575_GPIO_PIN_NUM;
                                  dev->params.addr += PCF8575_BASE_ADDR;
                                  break;
#endif
        default: return PCF857X_ERROR_INV_EXP;
    }

#if MODULE_PCF857X_IRQ
    /* initialize the IRQ event object used for delaying interrupts */
    dev->irq_event.event.handler = _irq_handler;
    dev->irq_event.dev = dev;

    for (unsigned i = 0; i < dev->pin_num; i++) {
        dev->isr[i].cb = NULL;
        dev->isr[i].arg = NULL;
        dev->enabled[i] = false;
    }

    /* initialize the interrupt pin */
    EXEC_RET(gpio_init_int(dev->params.int_pin, GPIO_IN, GPIO_FALLING,
                                                _irq_isr, (void*)dev));
#endif /* MODULE_PCF857X_IRQ */

    /* write 1 to all pins to switch them to INPUTS pulled up to HIGH */
    dev->out = ~0;
    EXEC_RET(_write(dev, dev->out));

    /* initial read all pins */
    EXEC_RET(_read(dev, &dev->in));

    /* set all pin modes to INPUT and set internal output data to 1 (HIGH) */
    dev->modes = ~0;

    return PCF857X_OK;
}

int pcf857x_gpio_init(pcf857x_t *dev, gpio_t pin, gpio_mode_t mode)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u mode=%u", dev, pin, mode);

    switch (mode) {
#ifndef MCU_CC2538
        case GPIO_OD:
        case GPIO_OD_PU:
#endif
        case GPIO_IN_PU:
        case GPIO_IN:    dev->modes |= (1 << pin); /* set mode bit to 1 */
                         dev->out   |= (1 << pin); /* set output bit to 1 */
                         break;
        case GPIO_OUT:   dev->modes &= ~(1 << pin); /* set mode bit to 0 */
                         dev->out   &= ~(1 << pin); /* set output bit to 0 */
                         break;
        default: DEBUG_DEV("gpio mode %d not supported", dev, mode);
                 return -PCF857X_ERROR_INV_MODE;
    }

    /* write the mode */
    pcf857x_data_t data = dev->modes | dev->out;
    EXEC_RET(_write(dev, data));

#if MODULE_PCF857X_IRQ
    /* reset the callback in case the port used external interrupts before */
    dev->isr[pin].cb = NULL;
    dev->isr[pin].arg = NULL;
    dev->enabled[pin] = false;

    /*
     * If an output of the expander is connected to an input of the same
     * expander, there is no interrupt triggered by the input when the
     * output changes.
     * Therefore, we have to read input pins after the write operation to
     * update the input pin state in the device data structure and to trigger
     * an ISR if necessary.
     */
    _update_state(dev);
#else
    /* read to update the internal input image */
    EXEC_RET(_read(dev, &dev->in));
#endif

    return PCF857X_OK;
}

#if MODULE_PCF857X_IRQ
int pcf857x_gpio_init_int(pcf857x_t *dev, gpio_t pin,
                                          gpio_mode_t mode,
                                          gpio_flank_t flank,
                                          gpio_cb_t isr,
                                          void *arg)
{
    /* initialize the pin */
    EXEC_RET(pcf857x_gpio_init(dev, pin, mode));

    switch (flank) {
        case GPIO_FALLING:
        case GPIO_RISING:
        case GPIO_BOTH: dev->isr[pin].cb = isr;
                        dev->isr[pin].arg = arg;
                        dev->flank[pin] = flank;
                        dev->enabled[pin] = true;
                        break;
        default: DEBUG_DEV("invalid flank %d for pin %d", dev, flank, pin);
                 return -PCF857X_ERROR_INV_FLANK;
    }

    return PCF857X_OK;
}

void pcf857x_gpio_irq_enable(pcf857x_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u", dev, pin);
    dev->enabled[pin] = true;
}

void pcf857x_gpio_irq_disable(pcf857x_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u", dev, pin);
    dev->enabled[pin] = false;
}
#endif

int pcf857x_gpio_read (pcf857x_t *dev, gpio_t pin)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u", dev, pin);

    /*
     * If we use the interrupt, we always have an up-to-date input snapshot
     * stored in the device data structure and which can be used directly.
     * Otherwise we have to read the pins first.
     */
#ifndef MODULE_PCF857X_IRQ
    EXEC_RET(_read(dev, &dev->in));
#endif
    return (dev->in & (1 << pin)) ? 1 : 0;
}

void pcf857x_gpio_write (pcf857x_t *dev, gpio_t pin, int value)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u value=%d", dev, pin, value);

    /* set pin bit value */
    if (value) {
        dev->out |= (1 << pin);
    }
    else {
        dev->out &= ~(1 << pin);
    }

    /* update pin values */
    pcf857x_data_t data = dev->modes | dev->out;
    EXEC(_write(dev, data));
#if MODULE_PCF857X_IRQ
    /*
     * If an output of the expander is connected to an input of the same
     * expander, there is no interrupt triggered by the input when the
     * output changes.
     * Therefore, we have to read input pins after the write operation to
     * update the input pin state in the device data structure and to trigger
     * an ISR if necessary.
     */
    _update_state(dev);
#endif
}

void pcf857x_gpio_clear (pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, 0);
}

void pcf857x_gpio_set (pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, 1);
}

void pcf857x_gpio_toggle (pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, (dev->out & (1 << pin)) ? 0 : 1);
}

/** Functions for internal use only */

#if MODULE_PCF857X_IRQ

/* interrupt service routine for IRQs */
static void _irq_isr(void *arg)
{
    assert(arg != NULL);

    /* just indicate that an interrupt occured and return */
    event_post(EVENT_PRIO_MEDIUM, (event_t*)&((pcf857x_t*)arg)->irq_event);
}

/* handle one IRQ event of device referenced by the event */
static void _irq_handler(event_t* event)
{
    pcf857x_irq_event_t* irq_event = (pcf857x_irq_event_t*)event;

    assert(irq_event != NULL);
    _update_state(irq_event->dev);
}

static void _update_state(pcf857x_t* dev)
{
    assert(dev != NULL);
    DEBUG_DEV("", dev);

    uint32_t state = irq_disable();

    /* save old input values */
    pcf857x_data_t old_in = dev->in;
    pcf857x_data_t new_in;

    /* read in new input values */
    EXEC(_read(dev, &dev->in));
    new_in = dev->in;

    irq_restore(state);

    /* iterate over all pins to check whether ISR has to be called */
    for (unsigned i = 0; i < dev->pin_num; i++) {
        pcf857x_data_t mask = 1 << i;

        /*
         * if pin is input, interrupt is enabled, has an ISR registered
         * and the input value changed
         */
        if ((dev->modes & mask) != 0 && dev->enabled[i] &&
            (dev->isr[i].cb != NULL) && (old_in & mask) != (new_in & mask)) {

            /* check for the flank and the activated flank mode */
            if (dev->flank[i] == GPIO_BOTH  ||  /* no matter what flank */
                ((new_in & mask) == 0 &&        /* falling flank */
                 (dev->flank[i] == GPIO_FALLING)) ||
                ((new_in & mask) == mask &&     /* rising flank */
                 (dev->flank[i] == GPIO_RISING))) {

                /* call the ISR */
                dev->isr[i].cb(dev->isr[i].arg);
            }
        }
    }
}
#endif /* MODULE_PCF857X_IRQ */

static int _read(const pcf857x_t *dev, pcf857x_data_t *data)
{
    assert(dev != NULL);
    assert(data != NULL);

    if (i2c_acquire(dev->params.dev)) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -PCF857X_ERROR_I2C;
    }

    uint8_t bytes[2];
    size_t len = (dev->pin_num == 8) ? 1 : 2;

    int res = i2c_read_bytes(dev->params.dev, dev->params.addr, bytes, len, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not read data, reason %d (%s)",
                  dev, res, strerror(res * -1));
        return -PCF857X_ERROR_I2C;
    }

    if (dev->pin_num == 8) {
        *data = bytes[0];
        DEBUG_DEV("data=%02x", dev, *data);
    }
    else {
        *data = (bytes[1] << 8) | bytes[0];
        DEBUG_DEV("data=%04x", dev, *data);
    }

    return res;
}

static int _write(const pcf857x_t *dev, pcf857x_data_t data)
{
    assert(dev != NULL);

    uint8_t bytes[2];
    size_t len = 1;

    if (dev->pin_num == 8) {
        DEBUG_DEV("data=%02x", dev, data & 0xff);

        bytes[0] = data & 0xff;
        len = 1;
    }
    else {
        DEBUG_DEV("data=%04x", dev, data);

        bytes[0] = data & 0xff;
        bytes[1] = data >> 8;
        len = 2;
    }

    if (i2c_acquire(dev->params.dev) != 0) {
        DEBUG_DEV("could not aquire I2C bus", dev);
        return -PCF857X_ERROR_I2C;
    }

    int res = i2c_write_bytes(dev->params.dev, dev->params.addr, bytes, len, 0);
    i2c_release(dev->params.dev);

    if (res != 0) {
        DEBUG_DEV("could not write data, reason %d (%s)",
                  dev, res, strerror(res * -1));
       return -PCF857X_ERROR_I2C;
    }

    return res;
}
