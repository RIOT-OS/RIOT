/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     lpc2387
 * @{
 *
 * @file
 * @brief       CPU specific low-level GPIO driver implementation for the LPC2387
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "irq.h"
#include "periph/gpio.h"
#include "bitfield.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define GPIO_NUM_ISR    (16)

static BITFIELD(_gpio_config_bitfield, GPIO_NUM_ISR);

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

static gpio_state_t _gpio_states[GPIO_NUM_ISR];
static BITFIELD(_gpio_rising, GPIO_NUM_ISR);
static BITFIELD(_gpio_falling, GPIO_NUM_ISR);
static uint8_t _gpio_isr_map[64]; /* only ports 0+2 can have ISRs */

static void _gpio_configure(gpio_t pin, unsigned rising, unsigned falling);

void gpio_init_ports(void) {
    SCS |= 0x1; /* set GPIO ports 0 and 1 to FIO mode (3.7.2) */
    memset(&_gpio_isr_map[0], 0xff, 64);
}

static int _isr_map_entry(gpio_t pin) {
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;

    /* lpc2387 can only interrupt in pins of port 0 and 2 */
    if (port && port != 2) {
        return -1;
    }

    if (port) {
        _pin += 32;
    }

    return _pin;
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pullup)
{
    (void) dir;
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;

    FIO_PORT_t *_port = &FIO_PORTS[port];

    /* set mask */
    _port->MASK = ~(0x1<<_pin);

    /* set direction */
    _port->DIR = ~0;

    /* set pullup/pulldown **/
    PINMODE[pin>>4] |= pullup << (_pin*2);

    gpio_init_mux(pin, 0);

    return 0;
}

int gpio_init_mux(unsigned pin, unsigned mux)
{
    (void) mux;
    unsigned _pin = pin & 31;
    PINSEL[pin>>4] &= ~(0x1 << (_pin*2));
    return 0;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pullup, gpio_flank_t flank,
                    gpio_cb_t cb, void *arg)
{
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
        puts("lpc2387: gpio: warning: no free gpio callback state!");
#endif
        return -1;
    }

    /* store callback */
    _gpio_states[_state_index].cb = cb;
    _gpio_states[_state_index].arg = arg;

    extern void GPIO_IRQHandler(void);
    gpio_init(pin, GPIO_DIR_IN, pullup);

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

    unsigned state = disableIRQ();

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

    restoreIRQ(state);
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

int gpio_read(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    _port->MASK = ~(1<<_pin);
    return _port->PIN != 0;
}

void gpio_set(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    _port->MASK = ~(1<<_pin);
    _port->SET = ~0;
}

void gpio_clear(gpio_t pin)
{
    unsigned _pin = pin & 31;
    unsigned port = pin >> 5;
    FIO_PORT_t *_port = &FIO_PORTS[port];
    _port->MASK = ~(1<<_pin);
    _port->CLR = ~0;
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

void gpio_write(gpio_t dev, int value)
{
    if (value) {
        gpio_set(dev);
    }
    else {
        gpio_clear(dev);
    }
}

static void test_irq(int port, unsigned long f_mask, unsigned long r_mask)
{
    /* Test each bit of rising and falling masks, if set trigger interrupt
     * on corresponding device */
    unsigned bit = 0x1;
    int n = 0;
    while (bit) {
        if ((r_mask & bit) | (f_mask & bit)) {
            int _state_index = _gpio_isr_map[n + (port<<1)];
            if (_state_index != 0xff) {
                _gpio_states[_state_index].cb(_gpio_states[_state_index].arg);
            }
        }

        bit <<= 1;
        n++;
    }
}

void GPIO_IRQHandler(void) __attribute__((interrupt("IRQ")));

void GPIO_IRQHandler(void)
{
    if (IO_INT_STAT & BIT0) {                       /* interrupt(s) on PORT0 pending */
        unsigned long int_stat_f = IO0_INT_STAT_F;  /* save content */
        unsigned long int_stat_r = IO0_INT_STAT_R;  /* save content */

        IO0_INT_CLR = int_stat_f;                   /* clear flags of fallen pins */
        IO0_INT_CLR = int_stat_r;                   /* clear flags of risen pins */

        test_irq(0, int_stat_f, int_stat_r);
    }

    if (IO_INT_STAT & BIT2) {                       /* interrupt(s) on PORT2 pending */
        unsigned long int_stat_f = IO2_INT_STAT_F;  /* save content */
        unsigned long int_stat_r = IO2_INT_STAT_R;  /* save content */

        IO2_INT_CLR = int_stat_f;                   /* clear flags of fallen pins */
        IO2_INT_CLR = int_stat_r;                   /* clear flags of risen pins */

        test_irq(2, int_stat_f, int_stat_r);
    }

    VICVectAddr = 0;                                /* Acknowledge Interrupt */
}
