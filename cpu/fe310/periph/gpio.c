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
#include "plic.h"
#include "vendor/riscv_csr.h"
#include "vendor/platform.h"

/* Num of GPIOs supported */
#define GPIO_NUMOF (32)

#ifdef MODULE_PERIPH_GPIO_IRQ
static gpio_flank_t isr_flank[GPIO_NUMOF];
static gpio_isr_ctx_t isr_ctx[GPIO_NUMOF];
#endif /* MODULE_PERIPH_GPIO_IRQ */

/* Really always inline these functions These two should be only a few
 * instructions as the atomic_fetch_or is a single instruction on rv32imac */
static __attribute((always_inline)) inline
void _set_pin_reg(uint32_t offset, gpio_t pin)
{
    __atomic_fetch_or(&GPIO_REG(offset), 1 << pin, __ATOMIC_RELAXED);
}

static __attribute((always_inline)) inline
void _clr_pin_reg(uint32_t offset, gpio_t pin)
{
    __atomic_fetch_and(&GPIO_REG(offset), ~(1 << pin), __ATOMIC_RELAXED);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return -1;
    }

    /*  Configure the mode */

    switch (mode) {
    case GPIO_IN:
        _set_pin_reg(GPIO_INPUT_EN, pin);
        _clr_pin_reg(GPIO_OUTPUT_EN, pin);
        _clr_pin_reg(GPIO_PULLUP_EN, pin);
        break;

    case GPIO_IN_PU:
        _clr_pin_reg(GPIO_OUTPUT_EN, pin);
        _set_pin_reg(GPIO_INPUT_EN, pin);
        _set_pin_reg(GPIO_PULLUP_EN, pin);
        break;

    case GPIO_OUT:
        _set_pin_reg(GPIO_OUTPUT_EN, pin);
        _clr_pin_reg(GPIO_INPUT_EN, pin);
        _clr_pin_reg(GPIO_PULLUP_EN, pin);
        break;

    default:
        return -1;
    }

    /* Configure the pin muxing for the GPIO */
    _clr_pin_reg(GPIO_IOF_EN, pin);
    _clr_pin_reg(GPIO_IOF_SEL, pin);

    return 0;
}

int gpio_read(gpio_t pin)
{
    return (GPIO_REG(GPIO_INPUT_VAL) & (1 << pin)) ? 1 : 0;
}

void gpio_set(gpio_t pin)
{
    _set_pin_reg(GPIO_OUTPUT_VAL, pin);
}

void gpio_clear(gpio_t pin)
{
    _clr_pin_reg(GPIO_OUTPUT_VAL, pin);
}

void gpio_toggle(gpio_t pin)
{
    __atomic_fetch_xor(&GPIO_REG(GPIO_OUTPUT_VAL), (1 << pin),
                       __ATOMIC_RELAXED);
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        _set_pin_reg(GPIO_OUTPUT_VAL, pin);
    }
    else {
        _clr_pin_reg(GPIO_OUTPUT_VAL, pin);
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
        _set_pin_reg(GPIO_FALL_IP, pin);
        break;

    case GPIO_RISING:
        _set_pin_reg(GPIO_RISE_IP, pin);
        break;

    case GPIO_BOTH:
        _set_pin_reg(GPIO_FALL_IP, pin);
        _set_pin_reg(GPIO_RISE_IP, pin);
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
    plic_set_isr_cb(INT_GPIO_BASE + pin, gpio_isr);
    plic_enable_interrupt(INT_GPIO_BASE + pin);
    plic_set_priority(INT_GPIO_BASE + pin, GPIO_INTR_PRIORITY);

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
        _set_pin_reg(GPIO_FALL_IE, pin);
        break;

    case GPIO_RISING:
        _set_pin_reg(GPIO_RISE_IE, pin);
        break;

    case GPIO_BOTH:
        _set_pin_reg(GPIO_FALL_IE, pin);
        _set_pin_reg(GPIO_RISE_IE, pin);
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
        _clr_pin_reg(GPIO_FALL_IE, pin);
        break;

    case GPIO_RISING:
        _clr_pin_reg(GPIO_RISE_IE, pin);
        break;

    case GPIO_BOTH:
        _clr_pin_reg(GPIO_FALL_IE, pin);
        _clr_pin_reg(GPIO_RISE_IE, pin);
        break;

    default:
        break;
    }
}
#endif /* MODULE_PERIPH_GPIO_IRQ */
