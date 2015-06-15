/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} gpio_state_t;

/* disable unused PIOs in periph_conf.h to save some bytes */
#define PIOB_OFFSET  (PIOA_EN * PIOA_NUMOF)
#define PIOC_OFFSET  (PIOB_OFFSET + (PIOB_EN * PIOB_NUMOF))

static gpio_state_t pio_callback[PIOA_NUMOF + PIOB_NUMOF + PIOC_NUMOF];

/* 0 = PIOA, 1 = PIOB, 2 = PIOC */
static inline uint32_t _port_no(gpio_t pin)
{
    return ((pin & 0x60) >> 5);
}

/* PIO base addresses are spaced 512 Bytes apart */
static inline Pio *_port(gpio_t pin)
{
    return ((void *)PIOA + (_port_no(pin) * 512));
}

/* Returns the pin number relative its PIO */
static inline uint32_t _pin_no(gpio_t pin)
{
    return (pin & 0x1F);
}

/* Generates the mask for this pin */
static inline uint32_t _msk(gpio_t pin)
{
    return (1u << _pin_no(pin));
}

/* Calculates the IRQ number */
static inline uint32_t _irq(gpio_t pin)
{
    return (PIOA_IRQn + _port_no(pin));
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pushpull)
{
    Pio *port = _port(pin);
    uint32_t msk = _msk(pin);

    /* enable power on this pin */
    port->PIO_PER = msk;

    /* pull resistor configuration */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;

        case GPIO_PULLUP:
            port->PIO_PUER = msk;
            break;

        case GPIO_NOPULL:
            port->PIO_PUDR = msk;
            break;
    }

    /* configure pin direction */
    switch (dir) {
        case GPIO_DIR_OUT:
            port->PIO_OER = msk;
            port->PIO_CODR = msk;
            break;

        case GPIO_DIR_IN:
            port->PIO_ODR = msk;
            break;
    }

    return 0;
}

int gpio_init_exti(gpio_t pin, gpio_pp_t pullup,
                   gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    Pio *port = _port(pin);
    uint32_t msk = _msk(pin);
    uint32_t offset = 0;

    /* initialize port as input */
    if ((gpio_init(pin, GPIO_DIR_IN, pullup) < 0)) {
        return -1;
    }

    /* enable IRQ for this PIO controller */
    NVIC_SetPriority(_irq(pin), GPIO_IRQ_PRIO);
    NVIC_EnableIRQ(_irq(pin));

#if PIOB_EN

    if (_port_no(pin) == PB) {
        offset = PIOB_OFFSET;
    }

#endif

#if PIOC_EN

    if (_port_no(pin) == PC) {
        offset = PIOC_OFFSET;
    }

#endif

    /* store callback function and parameter */
    pio_callback[offset + _pin_no(pin)].cb = cb;
    pio_callback[offset + _pin_no(pin)].arg = arg;

    /* active flank configuration */
    switch (flank) {
        case GPIO_FALLING:
            port->PIO_AIMER = msk;
            port->PIO_ESR = msk;
            port->PIO_FELLSR = msk;
            break;

        case GPIO_RISING:
            port->PIO_AIMER = msk;
            port->PIO_ESR = msk;
            port->PIO_REHLSR = msk;
            break;

        case GPIO_BOTH:
            port->PIO_AIMDR = msk;
            break;
    }

    /* clear status register */
    port->PIO_ISR;

    /* enable the interrupt for the given channel */
    port->PIO_IER = msk;

    return 0;
}

int gpio_read(gpio_t pin)
{
    Pio *port = _port(pin);
    uint32_t msk = _msk(pin);
    uint32_t pin_no = _pin_no(pin);

    if (port->PIO_OSR & msk) {
        return *((int *)(BITBAND_REGADDR(port->PIO_ODSR, pin_no)));
    }
    else {
        return *((int *)(BITBAND_REGADDR(port->PIO_PDSR, pin_no)));
    }
}

void gpio_set(gpio_t pin)
{
    _port(pin)->PIO_SODR = _msk(pin);
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->PIO_CODR = _msk(pin);
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    }
    else {
        gpio_clear(pin);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    _port(pin)->PIO_IDR = _msk(pin);

    NVIC_DisableIRQ(_irq(pin));
}

void gpio_irq_enable(gpio_t pin)
{
    _port(pin)->PIO_IER = _msk(pin);

    NVIC_EnableIRQ(_irq(pin));
}

/* Template for the inner body of the PIO ISR */
static void isr(Pio *pio, uint32_t offset)
{
    uint32_t status = pio->PIO_ISR & pio->PIO_IMR;

    while (status) {
        uint32_t i = (offset + 31) - __CLZ(status);
        pio_callback[i].cb(pio_callback[i].arg);
        status &= ~(1 << i);
    }

    if (sched_context_switch_request) {
        thread_yield();
    }
}

#if PIOA_EN
void isr_pioa(void)
{
    isr(PIOA, 0);
}
#endif  /* PIOA_EN */

/* Not tested */
#if PIOB_EN
void isr_piob(void)
{
    isr(PIOB, PIOB_OFFSET);
}
#endif  /* PIOB_EN */

/* Not tested (no hardware with PIOC) */
#if PIOC_EN
void isr_pioc(void)
{
    isr(PIOC, PIOC_OFFSET);
}
#endif  /* PIOC_EN */
