/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup cpu_rp2040
 * @ingroup drivers_periph_gpio
 * @{
 *
 * @file gpio.c
 * @brief CPU specific low-level GPIO driver
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

static bool is_gpio_pin_sio(gpio_t pin);

static bool is_gpio_pin_sio(gpio_t pin) {
    uint32_t io_pin;
    uint32_t funcsel_pin;

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    funcsel_pin =
        (iobank0_hw->io[io_pin].ctrl & IO_BANK0_GPIO1_CTRL_FUNCSEL_BITS);

    return (funcsel_pin == IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_SIO_0);
}

int gpio_init(gpio_t pin, gpio_mode_t mode) {
    uint32_t io_pin;

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    resets_hw->reset &= ~(RESETS_RESET_IO_BANK0_BITS);
    resets_hw->reset &= ~(RESETS_RESET_PADS_BANK0_BITS);

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_IO_BANK0_BITS)) {}

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PADS_BANK0_BITS)) {}

    // Set pin function to SIO.
    iobank0_hw->io[io_pin].ctrl &= ~(IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS);
    iobank0_hw->io[io_pin].ctrl |= IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_SIO_0;

    if (mode == GPIO_IN) {
        padsbank0_hw->io[io_pin] |=
            (PADS_BANK0_GPIO0_OD_BITS | PADS_BANK0_GPIO0_IE_BITS);

        iobank0_hw->io[io_pin].ctrl &=
            ~(IO_BANK0_GPIO0_CTRL_OEOVER_BITS);
        iobank0_hw->io[io_pin].ctrl |=
            (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_DISABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);
    }
    else if (mode == GPIO_IN_PU) {
        padsbank0_hw->io[io_pin] |=
            (PADS_BANK0_GPIO0_OD_BITS | PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_PUE_BITS);

        padsbank0_hw->io[io_pin] &= ~(PADS_BANK0_GPIO0_PDE_BITS);

        iobank0_hw->io[io_pin].ctrl &=
            ~(IO_BANK0_GPIO0_CTRL_OEOVER_BITS);
        iobank0_hw->io[io_pin].ctrl |=
            (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_DISABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);
    }
    else if (mode == GPIO_IN_PD) {
        padsbank0_hw->io[io_pin] |=
            (PADS_BANK0_GPIO0_OD_BITS | PADS_BANK0_GPIO0_IE_BITS | PADS_BANK0_GPIO0_PDE_BITS);

        padsbank0_hw->io[io_pin] &= ~(PADS_BANK0_GPIO0_PUE_BITS);

        iobank0_hw->io[io_pin].ctrl &=
            ~(IO_BANK0_GPIO0_CTRL_OEOVER_BITS);
        iobank0_hw->io[io_pin].ctrl |=
            (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_DISABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);
    }
    else if (mode == GPIO_OUT) {
        padsbank0_hw->io[io_pin] &=
            (~(PADS_BANK0_GPIO0_OD_BITS) & ~(PADS_BANK0_GPIO0_IE_BITS));

        iobank0_hw->io[io_pin].ctrl &=
            ~(IO_BANK0_GPIO0_CTRL_OEOVER_BITS);
        iobank0_hw->io[io_pin].ctrl |=
            (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_ENABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);
    }
    else {
        return -1;
    }

    return 0;
}

int gpio_read(gpio_t pin) {
    uint32_t io_pin;
    uint32_t pad_read = 0;

    assert(is_gpio_pin_sio(pin));

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    if (!(padsbank0_hw->io[io_pin] & PADS_BANK0_GPIO0_OD_BITS)) {
        pad_read =
            (iobank0_hw->io[io_pin].status & IO_BANK0_GPIO1_STATUS_OUTTOPAD_BITS);
    }
    else if ((padsbank0_hw->io[io_pin] & PADS_BANK0_GPIO0_IE_BITS)) {
        pad_read =
            (iobank0_hw->io[io_pin].status & IO_BANK0_GPIO1_STATUS_INFROMPAD_BITS);
    }

    if (pad_read) {
        return 1;
    }
    else {
        return 0;
    }
}

void gpio_set(gpio_t pin) {
    uint32_t io_pin;

    assert(is_gpio_pin_sio(pin));

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    iobank0_hw->io[io_pin].ctrl &=
        ~(IO_BANK0_GPIO0_CTRL_OUTOVER_BITS);
    iobank0_hw->io[io_pin].ctrl |=
        (IO_BANK0_GPIO0_CTRL_OUTOVER_VALUE_HIGH << IO_BANK0_GPIO0_CTRL_OUTOVER_LSB);
}

void gpio_clear(gpio_t pin) {
    uint32_t io_pin;

    assert(is_gpio_pin_sio(pin));

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    iobank0_hw->io[io_pin].ctrl &=
        ~(IO_BANK0_GPIO0_CTRL_OUTOVER_BITS);
    iobank0_hw->io[io_pin].ctrl |=
        (IO_BANK0_GPIO0_CTRL_OUTOVER_VALUE_LOW << IO_BANK0_GPIO0_CTRL_OUTOVER_LSB);
}

void gpio_toggle(gpio_t pin) {
    uint32_t io_pin;
    uint32_t pad_read = 0;

    assert(is_gpio_pin_sio(pin));

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    if (!(padsbank0_hw->io[io_pin] & PADS_BANK0_GPIO0_OD_BITS)) {
        pad_read =
            (iobank0_hw->io[io_pin].status & IO_BANK0_GPIO1_STATUS_OUTTOPAD_BITS);
    }

    if (pad_read) {
        gpio_clear(pin);
    }
    else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value) {
    uint32_t io_pin;

    assert(is_gpio_pin_sio(pin));

    assert((pin >> 16) == GPIO_BANK_USER);

    io_pin = (pin & 0x0000ffff);

    if (!(padsbank0_hw->io[io_pin] & PADS_BANK0_GPIO0_OD_BITS)) {
        if (value == 0) {
            gpio_clear(pin);
        }
        else {
            gpio_set(pin);
        }
    }
}
