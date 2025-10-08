/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup         cpu_rp2350_riscv
 * @{
 *
 * @file
 * @brief           GPIO implementation for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "periph/gpio.h"

#include <errno.h>

#include "board.h"
#include "irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int gpio_init(gpio_t pin, gpio_mode_t mode) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);

    /* Clear the pin's output enable and output state */
    SIO->GPIO_OE_CLR = 1LU << pin;
    SIO->GPIO_OUT_CLR = 1LU << pin;

    switch (mode) {
        case GPIO_OUT:
        *(uint32_t*)calculate_gpio_io_ctrl_register_addr(pin) =
        FUNCTION_SELECT_SIO;

        volatile uint32_t* pad_reg =
        (uint32_t*)calculate_gpio_pad_register_addr(pin);

        /* We clear all bits except the drive strength bit.
         * We set that to the highest one possible (12mA)
         * to mimic the behavior of the pico1 GPIO driver
         * (Not too sure why we do this, but it seems to be the standard) */
        *pad_reg = 0x3 << 4;

        SIO->GPIO_OE_SET = 1 << pin; /* Set the pin as output */

        break;
        default:
        /* Unsupported mode */
        return -ENOTSUP;
    }
    return 0;
}

bool gpio_read(gpio_t pin) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);
    /* Read the pin state */
    return (SIO->GPIO_IN & (1 << pin)) != 0; /* Return true if the pin is HIGH */
}

void gpio_set(gpio_t pin) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);
    SIO->GPIO_OUT_SET = 1 << pin; /* Set the pin to HIGH */
}

void gpio_clear(gpio_t pin) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);
    SIO->GPIO_OUT_CLR = 1 << pin; /* Set the pin to LOW */
}

void gpio_toggle(gpio_t pin) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);
    SIO->GPIO_OUT_XOR = 1 << pin; /* Toggle the pin state (XOR) */
}

void gpio_write(gpio_t pin, bool value) {
    /* Check if we exceed the maximum number of GPIO pins */
    assert(pin < GPIO_PIN_NUMOF);
    if (value) {
        gpio_set(pin); /* Set the pin to HIGH */
    } else {
        gpio_clear(pin); /* Set the pin to LOW */
    }
}

/** @} */
