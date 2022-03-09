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

#if IS_USED(MODULE_PCF857X_IRQ)
#include "event/thread.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#if ENABLE_DEBUG

#define DEBUG_DEV(f, d, ...) \
        DEBUG("[pcf857x] %s i2c dev=%d addr=%02x: " f "\n", \
              __func__, d->params.dev, d->params.addr, ## __VA_ARGS__)

#else /* ENABLE_DEBUG */

#define DEBUG_DEV(f, d, ...)

#endif /* ENABLE_DEBUG */

#if IS_USED(MODULE_PCF857X_IRQ_LOW)
#define PCF857X_EVENT_PRIO EVENT_PRIO_LOWEST
#elif IS_USED(MODULE_PCF857X_IRQ_MEDIUM)
#define PCF857X_EVENT_PRIO EVENT_PRIO_MEDIUM
#elif IS_USED(MODULE_PCF857X_IRQ_HIGHEST)
#define PCF857X_EVENT_PRIO EVENT_PRIO_HIGHEST
#endif

/** Forward declaration of functions for internal use */

static inline void _acquire(const pcf857x_t *dev);
static inline void _release(const pcf857x_t *dev);
static int _read(const pcf857x_t *dev, pcf857x_data_t *data);
static int _write(const pcf857x_t *dev, pcf857x_data_t data);

#if IS_USED(MODULE_PCF857X_IRQ)

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
#if IS_USED(MODULE_PCF857X_IRQ)
    assert(gpio_is_valid(params->int_pin));
#endif

    DEBUG_DEV("params=%p", dev, params);

    /* init device data structure */
    dev->params = *params;

    switch (params->exp) {
#if IS_USED(MODULE_PCF8574)
        /**< PCF8574 8 bit I/O expander used */
        case PCF857X_EXP_PCF8574: dev->pin_num = PCF8574_GPIO_PIN_NUM;
                                  dev->params.addr += PCF8574_BASE_ADDR;
                                  break;
#endif
#if IS_USED(MODULE_PCF8574A)
        /**< PCF8574A 8 bit I/O expander */
        case PCF857X_EXP_PCF8574A: dev->pin_num = PCF8574A_GPIO_PIN_NUM;
                                   dev->params.addr += PCF8574A_BASE_ADDR;
                                   break;
#endif
#if IS_USED(MODULE_PCF8575)
        /**< PCF8575 16 bit I/O expander */
        case PCF857X_EXP_PCF8575: dev->pin_num = PCF8575_GPIO_PIN_NUM;
                                  dev->params.addr += PCF8575_BASE_ADDR;
                                  break;
#endif
        default: return -PCF857X_ERROR_INV_EXP;
    }

#if IS_USED(MODULE_PCF857X_IRQ)
    /* initialize the IRQ event object used for delaying interrupts */
    dev->irq_event.event.handler = _irq_handler;
    dev->irq_event.dev = dev;

    for (unsigned i = 0; i < dev->pin_num; i++) {
        dev->isr[i].cb = NULL;
        dev->isr[i].arg = NULL;
        dev->enabled[i] = false;
    }

    /* initialize the interrupt pin */
    if (gpio_init_int(dev->params.int_pin,
                      GPIO_IN_PU, GPIO_FALLING, _irq_isr, (void*)dev)) {
        return -PCF857X_ERROR_INT_PIN;
    }
#endif /* MODULE_PCF857X_IRQ */

    int res = PCF857X_OK;

    _acquire(dev);

    /* write 1 to all pins to switch them to INPUTS pulled up to HIGH */
    dev->out = ~0;
    res |= _write(dev, dev->out);

    /* initial read all pins */
    res |= _read(dev, &dev->in);

    /* set all pin modes to INPUT and set internal output data to 1 (HIGH) */
    dev->modes = ~0;

    _release(dev);

    return PCF857X_OK;
}

int pcf857x_gpio_init(pcf857x_t *dev, gpio_t pin, gpio_mode_t mode)
{
    /* some parameter sanity checks */
    assert(dev != NULL);
    assert(pin < dev->pin_num);

    DEBUG_DEV("pin=%u mode=%u", dev, pin, mode);

    /*
     * Since the LOW output is the only actively driven level possible with
     * this expander, only in the case of GPIO_OUT we write a 0 to the pin
     * to configure the pin as an output and actively drive it LOW. In all
     * other modes, the pin is configured as an input and pulled-up to HIGH
     * with the weak pull-up to emulate them.
     */
    switch (mode) {
        case GPIO_IN_PD: DEBUG_DEV("gpio mode GPIO_IN_PD not supported", dev, mode);
                         return -PCF857X_ERROR_INV_MODE;
        case GPIO_OUT:   dev->modes &= ~(1 << pin); /* set mode bit to 0 */
                         dev->out   &= ~(1 << pin); /* set output bit to 0 */
                         break;
        default:         dev->modes |= (1 << pin); /* set mode bit to 1 */
                         dev->out   |= (1 << pin); /* set output bit to 1 */
                         break;
    }

    int res;

    /* write the mode */
    pcf857x_data_t data = dev->modes | dev->out;
    _acquire(dev);
    if ((res = _write(dev, data)) != PCF857X_OK) {
        _release(dev);
        return res;
    }

#if IS_USED(MODULE_PCF857X_IRQ)
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
     *
     * @note _update_state releases the bus.
     */
    _update_state(dev);
#else
    /* read to update the internal input state */
    res = _read(dev, &dev->in);
    _release(dev);
#endif
    return res;
}

#if IS_USED(MODULE_PCF857X_IRQ)
int pcf857x_gpio_init_int(pcf857x_t *dev, gpio_t pin,
                                          gpio_mode_t mode,
                                          gpio_flank_t flank,
                                          gpio_cb_t isr,
                                          void *arg)
{
    int res = PCF857X_OK;

    /* initialize the pin */
    if ((res = pcf857x_gpio_init(dev, pin, mode)) != PCF857X_OK) {
        return res;
    }

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

int pcf857x_gpio_read(pcf857x_t *dev, gpio_t pin)
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
#if !IS_USED(MODULE_PCF857X_IRQ)
    _acquire(dev);
    _read(dev, &dev->in);
    _release(dev);
#endif
    return (dev->in &(1 << pin)) ? 1 : 0;
}

void pcf857x_gpio_write(pcf857x_t *dev, gpio_t pin, int value)
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
    _acquire(dev);
    _write(dev, data);
#if IS_USED(MODULE_PCF857X_IRQ)
    /*
     * If an output of the expander is connected to an input of the same
     * expander, there is no interrupt triggered by the input when the
     * output changes.
     * Therefore, we have to read input pins after the write operation to
     * update the input pin state in the device data structure and to trigger
     * an ISR if necessary.
     *
     * @note _update_state releases the bus.
     */
    _update_state(dev);
#else
    _release(dev);
#endif
}

void pcf857x_gpio_clear(pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, 0);
}

void pcf857x_gpio_set(pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, 1);
}

void pcf857x_gpio_toggle(pcf857x_t *dev, gpio_t pin)
{
    DEBUG_DEV("pin=%u", dev, pin);
    return pcf857x_gpio_write(dev, pin, (dev->out & (1 << pin)) ? 0 : 1);
}

/** Functions for internal use only */

#if IS_USED(MODULE_PCF857X_IRQ)

/* interrupt service routine for IRQs */
static void _irq_isr(void *arg)
{
    assert(arg != NULL);

    /* just indicate that an interrupt occurred and return */
    event_post(PCF857X_EVENT_PRIO, (event_t*)&((pcf857x_t*)arg)->irq_event);
}

/* handle one IRQ event of device referenced by the event */
static void _irq_handler(event_t* event)
{
    pcf857x_irq_event_t* irq_event = (pcf857x_irq_event_t*)event;

    assert(irq_event != NULL);
    _acquire(irq_event->dev);
    /* _update_state releases the bus */
    _update_state(irq_event->dev);
}

/*
 * @warning: It is expected that the I2C bus is already acquired when the
 *           function is called. However, it is released by this function
 *           before the function returns.
 */
static void _update_state(pcf857x_t* dev)
{
    assert(dev != NULL);
    DEBUG_DEV("", dev);

    /* save old input values */
    pcf857x_data_t old_in = dev->in;
    pcf857x_data_t new_in;

    /* read in new input values and release the bus */
    if (_read(dev, &dev->in)) {
        _release(dev);
        return;
    }
    _release(dev);

    new_in = dev->in;

    /* iterate over all pins to check whether ISR has to be called */
    for (unsigned i = 0; i < dev->pin_num; i++) {
        pcf857x_data_t mask = 1 << i;

        /*
         * if pin is input, interrupt is enabled, has an ISR registered
         * and the input value changed
         */
        if (((dev->modes & mask) != 0) && dev->enabled[i] &&
            (dev->isr[i].cb != NULL) && ((old_in ^ new_in) & mask)) {
            /* check for the flank and the activated flank mode */
            if ((dev->flank[i] == GPIO_BOTH) || /* no matter what flank */
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

static inline void _acquire(const pcf857x_t *dev)
{
    assert(dev != NULL);
    i2c_acquire(dev->params.dev);
}

static inline void _release(const pcf857x_t *dev)
{
    assert(dev != NULL);
    i2c_release(dev->params.dev);
}

static int _read(const pcf857x_t *dev, pcf857x_data_t *data)
{
    assert(dev != NULL);
    assert(data != NULL);

    uint8_t bytes[2];
    size_t len = (dev->pin_num == 8) ? 1 : 2;

    int res = i2c_read_bytes(dev->params.dev, dev->params.addr, bytes, len, 0);

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

    return PCF857X_OK;
}

static int _write(const pcf857x_t *dev, pcf857x_data_t data)
{
    assert(dev != NULL);

    uint8_t bytes[2];
    size_t len;

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

    int res = i2c_write_bytes(dev->params.dev, dev->params.addr, bytes, len, 0);

    if (res != 0) {
        DEBUG_DEV("could not write data, reason %d (%s)",
                  dev, res, strerror(res * -1));
       return -PCF857X_ERROR_I2C;
    }

    return PCF857X_OK;
}
