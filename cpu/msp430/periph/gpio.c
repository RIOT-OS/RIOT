/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "bitarithm.h"
#include "compiler_hints.h"
#include "container.h"
#include "periph/gpio.h"
#include "periph/gpio_ll.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
#include "modules.h"
#include "periph/gpio_ll_irq.h"
#endif

/**
 * @brief   Number of possible interrupt lines: 2 ports * 8 pins
 */
#define ISR_NUMOF           (16U)

/**
 * @brief   Number of pins on each port
 */
#define PINS_PER_PORT       (8U)

static msp430_port_t *_port(gpio_t pin)
{
    return (msp430_port_t *)gpio_port(pin >> 8);
}

static inline uint8_t _pin_mask(gpio_t pin)
{
    return (uint8_t)(pin & 0xff);
}

MAYBE_UNUSED
static uint8_t _pin_num(gpio_t pin)
{
    return bitarithm_lsb(_pin_mask(pin));
}

static inline msp430_port_p1_p2_t *_isr_port(gpio_t pin)
{
    /* checking for (pin >> 8) <= 2 requires 6 byte of .text more than
     * checking the resulting address */
    msp430_port_p1_p2_t *port = container_of(_port(pin), msp430_port_p1_p2_t, base);
    if ((port == &PORT_1) || (port == &PORT_2)) {
        return port;
    }

    return NULL;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    msp430_port_t *port = _port(pin);

    /* check if port is valid and mode applicable */
    if ((port == NULL) || ((mode != GPIO_IN) && (mode != GPIO_OUT))) {
        return -1;
    }

    /* set pin direction */
    if (mode == GPIO_OUT) {
        port->DIR |= _pin_mask(pin);
    }
    else {
        port->DIR &= ~(_pin_mask(pin));
    }

    return 0;
}

void gpio_periph_mode(gpio_t pin, bool enable)
{
    REG8 *sel;
    msp430_port_p1_p2_t *isrport = _isr_port(pin);
    if (isrport) {
        sel = &(isrport->SEL);
    }
    else {
        msp430_port_p3_p6_t *port = container_of(_port(pin), msp430_port_p3_p6_t, base);
        if (port) {
            sel = &(port->SEL);
        }
        else {
            return;
        }
    }
    if (enable) {
        *sel |= _pin_mask(pin);
    }
    else {
        *sel &= ~(_pin_mask(pin));
    }
}

bool gpio_read(gpio_t pin)
{
    msp430_port_t *port = _port(pin);
    if (port->DIR & _pin_mask(pin)) {
        return (int)(port->OD & _pin_mask(pin));
    }
    else {
        return (int)(port->IN & _pin_mask(pin));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->OD |= _pin_mask(pin);
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->OD &= ~(_pin_mask(pin));
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->OD ^= _pin_mask(pin);
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        _port(pin)->OD |= _pin_mask(pin);
    }
    else {
        _port(pin)->OD &= ~(_pin_mask(pin));
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
/**
 * @brief   Interrupt context for each interrupt line
 */
static gpio_isr_ctx_t isr_ctx[ISR_NUMOF];

static int _ctx(gpio_t pin)
{
    int i = _pin_num(pin);
    return (_port(pin) == &PORT_1.base) ? i : (i + 8);
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg)
{
    msp430_port_p1_p2_t *port = _isr_port(pin);

    /* check if port, pull resistor and flank configuration are valid */
    if ((port == NULL) || (flank == GPIO_BOTH)) {
        return -1;
    }

    /* disable any activated interrupt */
    port->IE &= ~(_pin_mask(pin));
    /* configure as input */
    if (gpio_init(pin, mode) < 0) {
        return -1;
    }

    if (IS_USED(MODULE_GPIO_LL_IRQ)) {
        gpio_irq_trig_t trig = (flank == GPIO_RISING) ? GPIO_TRIGGER_EDGE_RISING
                                                      : GPIO_TRIGGER_EDGE_FALLING;
        return gpio_ll_irq((gpio_port_t)&port->base, _pin_num(pin), trig, cb, arg);
    }
    else {
        /* save ISR context */
        isr_ctx[_ctx(pin)].cb = cb;
        isr_ctx[_ctx(pin)].arg = arg;
        /* configure flank */
        port->IES &= ~(_pin_mask(pin));
        port->IES |= (flank & _pin_mask(pin));
        /* clear pending interrupts and enable the IRQ */
        port->IFG &= ~(_pin_mask(pin));
        gpio_irq_enable(pin);
    }
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    msp430_port_p1_p2_t *port = _isr_port(pin);
    if (port) {
        port->IE |= _pin_mask(pin);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    msp430_port_p1_p2_t *port = _isr_port(pin);
    if (port) {
        port->IE &= ~(_pin_mask(pin));
    }
}

#  ifndef MODULE_GPIO_LL_IRQ
static inline void isr_handler(msp430_port_p1_p2_t *port, int ctx)
{
    for (unsigned i = 0; i < PINS_PER_PORT; i++) {
        if ((port->IE & (1 << i)) && (port->IFG & (1 << i))) {
            port->IFG &= ~(1 << i);
            isr_ctx[i + ctx].cb(isr_ctx[i + ctx].arg);
        }
    }
}

ISR(PORT1_VECTOR, isr_port1)
{
    __enter_isr();
    isr_handler(&PORT_1, 0);
    __exit_isr();
}

ISR(PORT2_VECTOR, isr_port2)
{
    __enter_isr();
    isr_handler(&PORT_2, 8);
    __exit_isr();
}
#  endif
#endif /* MODULE_PERIPH_GPIO_IRQ */
