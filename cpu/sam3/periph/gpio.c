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


#define GPIO_ISR_CHAN_NUMOF (4U)

typedef struct {
    gpio_cb_t cb;       /**< callback called from GPIO interrupt */
    void *arg;          /**< argument passed to the callback */
} exti_ctx_t;

static exti_ctx_t exti_ctx[GPIO_ISR_CHAN_NUMOF];

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

	gpio_init(pin, GPIO_DIR_IN, pushpull);

	/* set callback function and parameter */
    exti_ctx[port_num].cb = cb;
    exti_ctx[port_num].arg = arg;

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

void isr_exti(void)
{
    for (int i = 0; i < GPIO_ISR_CHAN_NUMOF; i++) {
        if (GPIO2(i)->PIO_ISR & 0xFFFFFFFF) {
            exti_ctx[i].cb(exti_ctx[i].arg);
        }
    }
    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_pioa(void)
{

	for(int i = 0; i < 32; i++) {
		if (GPIO2(0)->PIO_ISR & (1 << i)) {
			exti_ctx[0].cb(exti_ctx[0].arg);
		}
	}
	if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_piob(void)
{

	for(int i = 0; i < 32; i++) {
		if (GPIO2(1)->PIO_ISR & (1 << i)) {
			exti_ctx[1].cb(exti_ctx[1].arg);
		}
	}
	if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_pioc(void)
{

	for(int i = 0; i < 32; i++) {
		if (GPIO2(2)->PIO_ISR & (1 << i)) {
			exti_ctx[2].cb(exti_ctx[2].arg);
		}
	}
	if (sched_context_switch_request) {
        thread_yield();
        }
}

void isr_piod(void)
{

	for(int i = 0; i < 32; i++) {
		if (GPIO2(3)->PIO_ISR & (1 << i)) {
			exti_ctx[3].cb(exti_ctx[3].arg);
		}
	}
	if (sched_context_switch_request) {
        thread_yield();
        }
}
