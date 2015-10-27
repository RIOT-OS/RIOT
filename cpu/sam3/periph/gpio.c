/*
 * Copyright (C) 2014 Hauke Petersen <mail@haukepetersen.de>
 *               2015 Hamburg University of Applied Sciences
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Tobias Fredersdorf <tobias.fredersdorf@haw-hamburg.de>
 *
 * @}
 */

#include "cpu.h"
#include "sched.h"
#include "thread.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @brief We store 4 bit for each external interrupt line (each pin) that can
 *        mapped to an entry in the exti_ctx table
 */
#define EXTI_MAP_LENGTH     (16U)

/**
 * @brief We allow for 8 concurrent external interrupts to be set
 */
#define CTX_NUMOF           (8U)

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} exti_ctx_t;

static exti_ctx_t exti_ctx[CTX_NUMOF] = {
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL},
    {NULL, NULL}, {NULL, NULL}, {NULL, NULL}, {NULL, NULL}
};

static uint32_t exti_map[EXTI_MAP_LENGTH];

/**
 * @brief Extract the pin's port base address from the given pin identifier
 */
static inline Pio *_port(gpio_t pin)
{
    return (Pio *)(pin & ~(0x1f));
}

/**
 * @brief Extract the port number from the given pin identifier
 *
 * Isolating bits 9 to 12 of the port base addresses leads to unique port
 * numbers.
 */
static inline int _port_num(gpio_t pin)
{
    return (((pin >> 9) & 0x0f) - 7);
}

/**
 * @brief Get the pin number from the pin identifier, encoded in the LSB 5 bit
 */
static inline int _pin_num(gpio_t pin)
{
    return (pin & 0x1f);
}

/**
 * @brief Get context for a specific pin
 */
static inline int _ctx(int port, int pin)
{
    return (exti_map[(port * 4) + (pin >> 3)] >> ((pin & 0x7) * 4)) & 0xf;
}

/**
 * @brief Write an entry to the context map array
 */
static inline void write_map(int port, int pin, int ctx)
{
    exti_map[(port * 4) + (pin >> 3)] &= ~(0xf << ((pin & 0x7) * 4));
    exti_map[(port * 4) + (pin >> 3)] |=  (ctx << ((pin & 0x7) * 4));
}

/**
 * @brief Find a free spot in the array containing the interrupt contexts
 */
static int get_free_ctx(void)
{
    for (int i = 0; i < CTX_NUMOF; i++) {
        if (exti_ctx[i].cb == NULL) {
            return i;
        }
    }
    return -1;
}

int gpio_init(gpio_t pin, gpio_dir_t dir, gpio_pp_t pushpull)
{
    Pio *port = _port(pin);
    int pin_num = _pin_num(pin);
	int port_num = _port_num(pin);

	PMC->PMC_PCER0 = (1 << (port_num+11));

	 /* give the PIO module the power over the corresponding pin */
    port->PIO_PER = (1<<pin_num);
    /* configure the pin's pull resistor state */
    switch (pushpull) {
        case GPIO_PULLDOWN:
            return -1;
        case GPIO_PULLUP:
            port->PIO_PUER = (1<<pin_num);
            break;
        case GPIO_NOPULL:
            port->PIO_PUDR = (1<<pin_num);
            break;
    }
    if (dir == GPIO_DIR_OUT) {
        /* configure pin as output */
        port->PIO_OER = (1<<pin_num);
        port->PIO_CODR = (1<<pin_num);
    }
    else {
        /* configure pin as input */
        port->PIO_ODR = (1<<pin_num);
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_pp_t pushpull, gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
	Pio *port = _port(pin);
	int pin_num = _pin_num(pin);
    int port_num = _port_num(pin);

	port->PIO_IDR = (1<<pin_num);

    /* try go grab a free spot in the context array */
    int ctx_num = get_free_ctx();
    if (ctx_num < 0) {
        return -1;
    }
    /* save context */
    exti_ctx[ctx_num].cb = cb;
    exti_ctx[ctx_num].arg = arg;
    write_map(port_num, pin_num, ctx_num);

    /* set the active flank */
    switch (flank) {
        case GPIO_FALLING:
            port->PIO_AIMER = (1<<pin_num);
            port->PIO_ESR = (1<<pin_num);
            port->PIO_FELLSR =(1<<pin_num);
            break;
        case GPIO_RISING:
            port->PIO_AIMER = (1<<pin_num);
            port->PIO_ESR = (1<<pin_num);
            port->PIO_REHLSR = (1<<pin_num);
            break;
        case GPIO_BOTH:
            port->PIO_AIMDR = (1<<pin_num);
            break;
    }

	NVIC_EnableIRQ((1 << (port_num+11)));

    /* clean interrupt status register */
    port->PIO_ISR;

    /* enable the interrupt for the given channel */
    port->PIO_IER = (1<<pin_num);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    NVIC_EnableIRQ((1 << (_port_num(pin)+11)));
}

void gpio_irq_disable(gpio_t pin)
{
    NVIC_DisableIRQ((1 << (_port_num(pin)+11)));
}

int gpio_read(gpio_t pin)
{
    Pio *port = _port(pin);
    int pin_num = _pin_num(pin);
	int res;

     if (port->PIO_OSR & (1<<pin_num)) {
        res = port->PIO_ODSR & (1<<pin_num);
    }
    else {
        res = port->PIO_PDSR & (1<<pin_num);
    }

    /* make sure we are not returning a negative value if bit 31 is set */
    if (res < -1) {
        res = 1;
    }

	return res;
}

void gpio_set(gpio_t pin)
{
    _port(pin)->PIO_SODR = (1 << _pin_num(pin));
}

void gpio_clear(gpio_t pin)
{
    _port(pin)->PIO_CODR = (1 << _pin_num(pin));
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    } else {
        gpio_clear(pin);
    }
}

static inline void isr_handler(Pio *port, int port_num)
{
    uint32_t status = port->PIO_ISR;

    for (int i = 0; i < 32; i++) {
        if (status & (1 << i)) {
            int ctx = _ctx(port_num, i);
            exti_ctx[ctx].cb(exti_ctx[ctx].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_pioa(void)
{
    isr_handler(PIOA, PA);
}

void isr_piob(void)
{
    isr_handler(PIOB, PB);
}

void isr_pioc(void)
{
    isr_handler(PIOC, PC);
}

void isr_piod(void)
{
    isr_handler(PIOD, PD);
}
