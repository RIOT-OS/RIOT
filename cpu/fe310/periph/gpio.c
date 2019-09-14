/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO implementation
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdlib.h>
#include <unistd.h>

#include "irq.h"
#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"

/* Num of GPIOs supported */
#define GPIO_NUMOF (32)

#ifdef MODULE_PERIPH_GPIO_IRQ
static gpio_flank_t isr_flank[GPIO_NUMOF];
static gpio_isr_ctx_t isr_ctx[GPIO_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return -1;
    }

    /*  Configure the mode */
    switch (mode) {
        case GPIO_IN:
            GPIO_REG(GPIO_INPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << pin);
            break;

        case GPIO_IN_PU:
            GPIO_REG(GPIO_INPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) |= (1 << pin);
            break;

        case GPIO_OUT:
            GPIO_REG(GPIO_INPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << pin);
            break;

        default:
            return -1;
    }

    /* Configure the pin muxing for the GPIO */
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << pin);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << pin);

    return 0;
}

int gpio_read(gpio_t pin)
{
    return (GPIO_REG(GPIO_INPUT_VAL) & (1 << pin)) ? 1 : 0;
}

void gpio_set(gpio_t pin)
{
    GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << pin);
}

void gpio_clear(gpio_t pin)
{
    GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << pin);
}

void gpio_toggle(gpio_t pin)
{
    GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << pin);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << pin);
    }
    else {
        GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << pin);
    }
}

#ifdef MODULE_PERIPH_GPIO_IRQ
void gpio_isr(int num)
{
    uint32_t pin = num - INT_GPIO_BASE;

    /* Invoke callback function */
    if (isr_ctx[pin].cb) {
        isr_ctx[pin].cb(isr_ctx[pin].arg);
    }

    /* Clear interrupt */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IP) |= (1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IP) |= (1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IP) |= (1 << pin);
            GPIO_REG(GPIO_RISE_IP) |= (1 << pin);
            break;
    }
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* Configure pin */
    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    /* Disable ext interrupts when setting up */
    clear_csr(mie, MIP_MEIP);

    /* Configure GPIO ISR with PLIC */
    set_external_isr_cb(INT_GPIO_BASE + pin, gpio_isr);
    PLIC_enable_interrupt(INT_GPIO_BASE + pin);
    PLIC_set_priority(INT_GPIO_BASE + pin, GPIO_INTR_PRIORITY);

    /*  Configure the active flank(s) */
    gpio_irq_enable(pin);

    /* Save callback */
    isr_ctx[pin].cb = cb;
    isr_ctx[pin].arg = arg;
    isr_flank[pin] = flank;

    /* Re-eanble ext interrupts */
    set_csr(mie, MIP_MEIP);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return;
    }

    /* Enable interrupt for pin */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IE) |= (1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IE) |= (1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IE) |= (1 << pin);
            GPIO_REG(GPIO_RISE_IE) |= (1 << pin);
            break;

        default:
            break;
    }
}

void gpio_irq_disable(gpio_t pin)
{
    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return;
    }

    /* Disable interrupt for pin */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IE) &= ~(1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IE) &= ~(1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IE) &= ~(1 << pin);
            GPIO_REG(GPIO_RISE_IE) &= ~(1 << pin);
            break;

        default:
            break;
    }
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
