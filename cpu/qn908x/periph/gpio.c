/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_gpio
 *
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>

#include "cpu.h"
#include "bitarithm.h"
#include "periph/gpio.h"
#include "vectors_qn908x.h"
#include "gpio_mux.h"

#include "vendor/drivers/fsl_clock.h"
#include "vendor/drivers/fsl_iocon.h"

/* The pull-up / pull-down / high-z mode in the gpio_mode_t enum matches the
 * values in the IOCON_PinMuxSet() function.
 */
#if (GPIO_MODE(0, 0, 0) & 0x30) != IOCON_MODE_HIGHZ
#error "GPIO_MODE(x, y, 0) must be High-Z mode"
#endif
#if (GPIO_MODE(0, 0, 1) & 0x30) != IOCON_MODE_PULLDOWN
#error "GPIO_MODE(x, y, 0) must be pull-down mode"
#endif
#if (GPIO_MODE(0, 0, 2) & 0x30) != IOCON_MODE_PULLUP
#error "GPIO_MODE(x, y, 0) must be pull-up mode"
#endif

/* Bit mask indicating if a GPIO is set to open_drain. */
static uint32_t gpio_open_drain[GPIO_PORTS_NUMOF] = {};

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    GPIO_Type *const base = GPIO_T_ADDR(pin);
    const uint32_t mask = 1u << GPIO_T_PIN(pin);

    /* We need to enable the GPIO clock before we set any register in the GPIO
     * blocks. */
    CLOCK_EnableClock(kCLOCK_Gpio);

    /* Disable the interrupts just in case this was already configured as an
     * interrupt pin. Note: this only disables the pin(s) that you write a 1
     * to. */
    base->INTENCLR = mask;

    /* pin_mode is the "or" of the three parts: function, mode and drive
     * strength. The mode is just the bits 4 and 5 of the gpio_mode_t and
     * corresponds to the IOCON_MODE_* values */
    uint32_t pin_mode =
        IOCON_FUNC0 | /* FUNC0 is digital GPIO on all pins. */
        (mode & 0x30) | IOCON_DRIVE_HIGH;
    gpio_init_mux(pin, pin_mode);

    if (mode & 2) {
        /* output mode */
        /* Configure the open-drain variable for allowing setting the values
         * later. */
        if (mode & 1) {
            /* open-drain enabled. */
            gpio_open_drain[GPIO_T_PORT(pin)] |= mask;
            /* Starts with the pin set to "high" (open) in open-drain mode.
             * The DATAOUT value doesn't do anything if the output is not
             * enabled but we keep track of the current value in DATAOUT
             * anyway to allow gpio_toggle. */
            base->OUTENCLR = mask;
            base->DATAOUT |= mask;
        }
        else {
            gpio_open_drain[GPIO_T_PORT(pin)] &= ~mask;
            /* Starts with the pin set to low on push-pull mode. */
            base->DATAOUT &= ~mask;
            base->OUTENSET = mask;
        }
    }
    else {
        /* input mode */
        gpio_open_drain[GPIO_T_PORT(pin)] &= ~mask;
        base->OUTENCLR = mask;
    }

    return 0;
}

#ifdef MODULE_PERIPH_GPIO_IRQ

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_isr_cb_state_t;

/**
 * @brief   The number of GPIO pins per port.
 */
#define PINS_PER_PORT       (32)

/**
 * @brief   The total number of GPIO pins in the chip.
 */
#define TOTAL_GPIO_PINS     (35)

/* The IRQ number in the NVIC for each GPIO port. */
static const uint32_t gpio_nvic_irqs[GPIO_PORTS_NUMOF] = GPIO_IRQS;

static gpio_isr_cb_state_t gpio_isr_state[TOTAL_GPIO_PINS] = {};

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    if (flank == GPIO_BOTH) {
        /* GPIO_BOTH is not supported. */
        return -1;
    }
    uint8_t gpio_num = GPIO_T_PORT(pin) * PINS_PER_PORT + GPIO_T_PIN(pin);

    if (gpio_num >= TOTAL_GPIO_PINS) {
        return -1;
    }
    gpio_isr_state[gpio_num].cb = cb;
    gpio_isr_state[gpio_num].arg = arg;

    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    GPIO_Type *const base = GPIO_T_ADDR(pin);
    const uint32_t mask = 1u << GPIO_T_PIN(pin);

    switch (flank) {
        case GPIO_LOW:
            base->INTTYPECLR = mask;    /* CLR = level */
            base->INTPOLCLR = mask;     /* CLR = low */
            break;
        case GPIO_HIGH:
            base->INTTYPECLR = mask;    /* CLR = level */
            base->INTPOLSET = mask;     /* SET = high */
            break;
        case GPIO_FALLING:
            base->INTTYPESET = mask;    /* SET = edge */
            base->INTPOLCLR = mask;     /* CLR = falling */
            break;
        case GPIO_RISING:
            base->INTTYPESET = mask;    /* SET = edge */
            base->INTPOLSET = mask;     /* SET = rising */
            break;
        case GPIO_BOTH:
            /* Handled above */
            break;
    }
    NVIC_EnableIRQ(gpio_nvic_irqs[GPIO_T_PORT(pin)]);
    gpio_irq_enable(pin);
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    GPIO_T_ADDR(pin)->INTENSET = 1u << GPIO_T_PIN(pin);
}

void gpio_irq_disable(gpio_t pin)
{
    GPIO_T_ADDR(pin)->INTENCLR = 1u << GPIO_T_PIN(pin);
}

#endif /* defined(MODULE_PERIPH_GPIO_IRQ) */

int gpio_read(gpio_t pin)
{
    return ((GPIO_T_ADDR(pin)->DATA) >> GPIO_T_PIN(pin)) & 1u;
}

void gpio_set(gpio_t pin)
{
    GPIO_Type *const base = GPIO_T_ADDR(pin);
    const uint32_t mask = 1u << GPIO_T_PIN(pin);

    /* out_clr has only the pin bit set if this is an open-drain pin, which
     * means we need to disable the output. This needs to happen before changing
     * DATAOUT. */
    const uint32_t out_clr = mask & gpio_open_drain[GPIO_T_PORT(pin)];

    base->OUTENCLR = out_clr;

    base->DATAOUT |= mask;
}

void gpio_clear(gpio_t pin)
{
    GPIO_Type *const base = GPIO_T_ADDR(pin);
    const uint32_t mask = 1u << GPIO_T_PIN(pin);

    base->DATAOUT &= ~mask;

    /* out_clr has only the pin bit set if this is an open-drain pin, which
     * means we need to enable the output. This needs to happen after changing
     * DATAOUT. */
    const uint32_t out_clr = mask & gpio_open_drain[GPIO_T_PORT(pin)];
    base->OUTENSET = out_clr;
}

void gpio_toggle(gpio_t pin)
{
    GPIO_Type *const base = GPIO_T_ADDR(pin);
    const uint32_t mask = 1u << GPIO_T_PIN(pin);
    const uint32_t out_clr = mask & gpio_open_drain[GPIO_T_PORT(pin)];
    const uint32_t dataout = base->DATAOUT;

    /* The output is disabled if the pin is an open-drain pin and DATAOUT is
     * not set for that pin. This avoids having if conditions. */
    base->OUTENCLR = out_clr & ~dataout;
    base->DATAOUT ^= mask;
    /* The output is disabled if the pin is an open-drain and DATAOUT at the
     * beginning of the function was set. */
    base->OUTENSET = out_clr & dataout;
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

static inline void irq_handler(GPIO_Type *base, uint32_t port_num)
{
    uint32_t status = base->INTSTATUS;

    while (status) {
        /* Clear all the flags immediately and process them in order. This gives
         * a chance to execute every pin's interrupt handler even if another pin
         * is always on.
         * Note: to *clear* the interrupt flag you write a 1 to that bit.
         */
        base->INTSTATUS = status;
        while (status) {
            uint8_t pin;
            status = bitarithm_test_and_clear(status, &pin);
            uint32_t gpio_num = port_num * PINS_PER_PORT + pin;
            gpio_cb_t cb = gpio_isr_state[gpio_num].cb;
            if (cb) {
                cb(gpio_isr_state[gpio_num].arg);
            }
        }
        status = base->INTSTATUS;
    }
}

#ifdef GPIOA_BASE
void isr_gpioa(void)
{
    irq_handler(GPIOA, 0);
    cortexm_isr_end();
}
#endif /* GPIOA_BASE */

#ifdef GPIOB_BASE
void isr_gpiob(void)
{
    irq_handler(GPIOB, 1);
    cortexm_isr_end();
}
#endif  /* GPIOB_BASE */

#endif  /* MODULE_PERIPH_GPIO_IRQ */
