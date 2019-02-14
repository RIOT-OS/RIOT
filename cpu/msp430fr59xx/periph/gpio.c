/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fr59xx
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio.h"

/**
 * @brief   Number of possible interrupt lines: 4 ports * 8 pins
 */
#define ISR_NUMOF           32U

/**
 * @brief   Number of pins on each port
 */
#define PINS_PER_PORT       8U

/**
 * @brief   Interrupt context for each interrupt line
 */
static gpio_isr_ctx_t isr_ctx[ISR_NUMOF];

static msp_port_t *_port(gpio_t pin)
{
    switch (pin >> 8) {
        case P1:
        case P2:
            return PORT_A;
        case P3:
        case P4:
            return PORT_B;
        case PJ:
            return PORT_J;
        default:
            return NULL;
    }
}

static inline msp_port_t *_isr_port(gpio_t pin)
{
    msp_port_t *const p = _port(pin);

    if (p != PORT_J) {
        return p;
    }
    return NULL;
}

static inline uint8_t _pin(gpio_t pin)
{
    return (uint8_t)(pin & 0xff);
}

static inline uint8_t _port_index(gpio_t pin)
{
    return 1 - ((pin >> 8) % 2);
}

static int _ctx(gpio_t pin)
{
    const int i = bitarithm_lsb(_pin(pin));

    return ((pin >> 8) - 1) * PINS_PER_PORT + i;
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    msp_port_t *const port = _port(pin);

    /* check if port is valid and mode applicable */
    if ((port == NULL) || ((mode != GPIO_IN) && (mode != GPIO_OUT))) {
        return -1;
    }

    const uint8_t portindex = _port_index(pin);

    /* set pin direction */
    if (mode == GPIO_OUT) {
        port->DIR[portindex] |= _pin(pin);
    }
    else {
        port->DIR[portindex] &= ~(_pin(pin));
    }

    return 0;
}


void gpio_irq_enable(gpio_t pin)
{
    msp_port_t *const port = _isr_port(pin);

    if (port) {
        port->IE[_port_index(pin)] |= _pin(pin);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    msp_port_t *const port = _isr_port(pin);

    if (port) {
        port->IE[_port_index(pin)] &= ~(_pin(pin));
    }
}

int gpio_read(gpio_t pin)
{
    msp_port_t *const port = _port(pin);
    const uint8_t portindex = _port_index(pin);

    if (port->DIR[portindex] & _pin(pin)) {
        return (int)(port->OD[portindex] & _pin(pin));
    }
    else {
        return (int)(port->IN[portindex] & _pin(pin));
    }
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    msp_port_t *const port = _isr_port(pin);

    /* check if port, pull resistor and flank configuration are valid */
    if (port == NULL) {
        return -1;
    }

    const uint8_t portindex = _port_index(pin);

    /* disable any activated interrupt */
    port->IE[portindex] &= ~(_pin(pin));

    /* configure as input */
    if (gpio_init(pin, mode) < 0) {
        return -1;
    }

    /* save ISR context */
    gpio_isr_ctx_t *const ctx = &isr_ctx[_ctx(pin)];
    ctx->cb = cb;
    ctx->arg = arg;
    ctx->both_edges = (flank == GPIO_BOTH);

    /* configure flank */
    if (ctx->both_edges) {
        port->REN[portindex] &= ~_pin(pin);
        port->OD[portindex] &= ~_pin(pin);
    }
    else {
        port->REN[portindex] |= _pin(pin);
        port->OD[portindex] |= _pin(pin);
    }

    if (flank != GPIO_RISING) {
        port->IES[portindex] |= _pin(pin);
    }
    else {
        port->IES[portindex] &= ~_pin(pin);
    }

    /* clear pending interrupts and enable the IRQ */
    port->IFG[portindex] &= ~_pin(pin);
    gpio_irq_enable(pin);

    return 0;
}

void gpio_periph_mode(gpio_t pin, bool sel0, bool sel1, bool selComp)
{
    msp_port_t *const port = _port(pin);
    const uint8_t portindex = _port_index(pin);

    if (port) {

        if (sel0) {
            port->SEL0[portindex] |= _pin(pin);
        }
        else {
            port->SEL0[portindex] &= ~_pin(pin);
        }

        if (sel1) {
            port->SEL1[portindex] |= _pin(pin);
        }
        else {
            port->SEL1[portindex] &= ~_pin(pin);
        }

        if (selComp) {
            port->SELC[portindex] |= _pin(pin);
        }
        else {
            port->SELC[portindex] &= ~_pin(pin);
        }
    }
}


void gpio_set(gpio_t pin)
{
    _port(pin)->OD[_port_index(pin)] |= _pin(pin);
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->OD[_port_index(pin)] &= ~(_pin(pin));
}

void gpio_toggle(gpio_t pin)
{
    _port(pin)->OD[_port_index(pin)] ^= _pin(pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _port(pin)->OD[_port_index(pin)] |= _pin(pin);
    }
    else {
        _port(pin)->OD[_port_index(pin)] &= ~(_pin(pin));
    }
}

void gpio_reset_edge(gpio_t pin)
{
    _port(pin)->IES[_port_index(pin)] |= _pin(pin);
}

static inline void isr_handler(msp_port_t *port, uint8_t portindex, int ctx)
{
    for (unsigned i = 0; i < PINS_PER_PORT; i++) {
        if ((port->IE[portindex] & (1 << i)) && (port->IFG[portindex] & (1 << i))) {
            const gpio_isr_ctx_t *const ictx = &isr_ctx[i + ctx];

            if (ictx->both_edges) {
                port->IES[portindex] ^= (1 << i);
            }

            ictx->cb(ictx->arg);

            port->IFG[portindex] &= ~(1 << i);
        }
    }
}

ISR(PORT1_VECTOR, isr_port1)
{
    __enter_isr();
    isr_handler(PORT_A, 0, 8 * 0);
    __exit_isr();
}

ISR(PORT2_VECTOR, isr_port2)
{
    __enter_isr();
    isr_handler(PORT_A, 1, 8 * 1);
    __exit_isr();
}

ISR(PORT3_VECTOR, isr_port3)
{
    __enter_isr();
    isr_handler(PORT_B, 0, 8 * 2);
    __exit_isr();
}

ISR(PORT4_VECTOR, isr_port4)
{
    __enter_isr();
    isr_handler(PORT_B, 1, 8 * 3);
    __exit_isr();
}
