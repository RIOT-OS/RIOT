/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_lpc23xx
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       CPU specific low-level GPIO driver implementation for the LPC23XX family
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "periph/gpio.h"
#include "bitarithm.h"
#include "bitfield.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
#define GPIO_NUM_ISR    (16)

static BITFIELD(_gpio_config_bitfield, GPIO_NUM_ISR);

static gpio_isr_ctx_t _gpio_states[GPIO_NUM_ISR];
static BITFIELD(_gpio_rising, GPIO_NUM_ISR);
static BITFIELD(_gpio_falling, GPIO_NUM_ISR);
static uint8_t _gpio_isr_map[64]; /* only ports 0+2 can have ISRs */

static void _gpio_configure(gpio_t pin, unsigned rising, unsigned falling);

static inline int _isr_map_entry2(unsigned port, unsigned pin)
{
    return pin + (port ? 32 : 0);
}

static int _isr_map_entry(gpio_t pin) {
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;

    /* lpc23xx can only interrupt in pins of port 0 and 2 */
    if (port && port != 2) {
        return -1;
    }

    return _isr_map_entry2(port, _pin);
}
#endif /* MODULE_PERIPH_GPIO_IRQ */

void gpio_init_ports(void) {
    SCS |= 0x1; /* set GPIO ports 0 and 1 to FIO mode (3.7.2) */
#ifdef MODULE_PERIPH_GPIO_IRQ
    memset(&_gpio_isr_map[0], 0xff, 64);
#endif /* MODULE_PERIPH_GPIO_IRQ */
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];

    /* set direction */
    switch (mode){
        case GPIO_OUT:
            _port->DIR |= 1<<_pin;
            break;
        case GPIO_IN:
            _port->DIR &= ~(1<<_pin);
            break;
        default:
            return -1;
    }

    gpio_init_mux(pin, 0);

    return 0;
}

int gpio_init_mux(unsigned pin, unsigned mux)
{
    (void) mux;
    unsigned pos = (pin & 0xf) << 1;
    PINSEL[pin>>4] &= ~(0x3 << pos);
    return 0;
}

bool gpio_read(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    return (_port->PIN & (1 << _pin)) != 0;
}

void gpio_set(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    _port->SET = 1 << _pin;
}

void gpio_clear(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    _port->CLR = 1 << _pin;
}

void gpio_toggle(gpio_t dev)
{
    if (gpio_read(dev)) {
        gpio_clear(dev);
    }
    else {
        gpio_set(dev);
    }
}

void gpio_write(gpio_t pin, bool value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static void _gpio_configure(gpio_t pin, unsigned rising, unsigned falling)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;

    /* set irq settings */
    volatile unsigned long *en_f;
    volatile unsigned long *en_r;
    volatile unsigned long *en_clr;

    if (!port) {
            en_f = &IO0_INT_EN_F;
            en_r = &IO0_INT_EN_R;
            en_clr = &IO0_INT_CLR;
    }
    else {
            en_f = &IO2_INT_EN_F;
            en_r = &IO2_INT_EN_R;
            en_clr = &IO2_INT_CLR;
    }

    /* configure irq */
    unsigned int bit = 0x1 << _pin;

    unsigned state = irq_disable();

    *en_clr |= bit;                                         /* clear interrupt */

    if (falling) {
        *en_f |= bit;                                       /* enable falling edge */
    }
    else {
        *en_f &= ~bit;                                      /* disable falling edge */
    }

    if (rising) {
        *en_r |= bit;                                       /* enable rising edge */
    }
    else {
        *en_r &= ~bit;                                      /* disable rising edge */
    }

    irq_restore(state);
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    (void)mode;

    DEBUG("gpio_init_int(): pin %u\n", pin);
    int isr_map_entry;

    /* check if interrupt is possible for this pin */
    if ((isr_map_entry = _isr_map_entry(pin)) == -1) {
        DEBUG("gpio_init_int(): pin %u cannot be used to generate interrupts.\n", pin);
        return -1;
    }

    /* find free isr state entry */
    int _state_index = _gpio_isr_map[isr_map_entry];

    if (_state_index == 0xff) {
        _state_index = bf_get_unset(_gpio_config_bitfield, GPIO_NUM_ISR);
        _gpio_isr_map[isr_map_entry] = _state_index;
        DEBUG("gpio_init_int(): pin has state_index=%i isr_map_entry=%i\n", _state_index, isr_map_entry);
    }

    if (_state_index == 0xff) {
#ifdef DEVELHELP
        puts("lpc23xx: gpio: warning: no free gpio callback state!");
#endif
        return -1;
    }

    /* store callback */
    _gpio_states[_state_index].cb = cb;
    _gpio_states[_state_index].arg = arg;

    extern void GPIO_IRQHandler(void);

    if (flank & GPIO_FALLING) {
        bf_set(_gpio_falling, _state_index);
    }
    else {
        bf_unset(_gpio_falling, _state_index);
    }

    if (flank & GPIO_RISING) {
        bf_set(_gpio_rising, _state_index);
    }
    else {
        bf_unset(_gpio_rising, _state_index);
    }

    _gpio_configure(pin, flank & GPIO_RISING, flank & GPIO_FALLING);

    /* activate irq */
    INTWAKE |= GPIO0WAKE | GPIO2WAKE;                       /* allow GPIO to wake up from power down */
    install_irq(GPIO_INT, &GPIO_IRQHandler, IRQP_GPIO);     /* install irq handler */

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    int isr_map_entry =_isr_map_entry(pin);
    int _state_index = _gpio_isr_map[isr_map_entry];

    if (_state_index == 0xff) {
        DEBUG("gpio_irq_enable(): trying to enable unconfigured pin.\n");
        return;
    }

    _gpio_configure(pin,
            bf_isset(_gpio_rising, _state_index),
            bf_isset(_gpio_falling, _state_index));
}

void gpio_irq_disable(gpio_t dev)
{
    _gpio_configure(dev, 0, 0);
}

static void test_irq(int port, unsigned long active_pins)
{
    /* Test each bit of rising and falling masks, if set trigger interrupt
     * on corresponding device */

    uint8_t pin = 0;
    while (active_pins) {
        /* get the index of the next set pin */
        active_pins = bitarithm_test_and_clear(active_pins, &pin);

        /* get the index of the configured interrupt */
        int _state_index = _gpio_isr_map[_isr_map_entry2(port, pin)];

        /* check if interrupt is configured */
        if (_state_index != 0xff) {
            _gpio_states[_state_index].cb(_gpio_states[_state_index].arg);
        }
    }
}

void GPIO_IRQHandler(void) __attribute__((interrupt("IRQ")));

void GPIO_IRQHandler(void)
{
    unsigned long int_stat;

    if (IO_INT_STAT & BIT0) {                       /* interrupt(s) on PORT0 pending */
        int_stat = IO0_INT_STAT_F | IO0_INT_STAT_R; /* get risen & fallen pin IRQs */
        IO0_INT_CLR = int_stat;                     /* clear IRQ flags */
        test_irq(0, int_stat);
    }

    if (IO_INT_STAT & BIT2) {                       /* interrupt(s) on PORT2 pending */
        int_stat = IO2_INT_STAT_F | IO2_INT_STAT_R; /* get risen & fallen pin IRQs */
        IO2_INT_CLR = int_stat;                     /* clear IRQ flags */
        test_irq(2, int_stat);
    }

    VICVectAddr = 0;                                /* Acknowledge Interrupt */
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
