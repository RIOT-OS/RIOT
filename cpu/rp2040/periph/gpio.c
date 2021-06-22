/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_rp2040
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       CPU specific low-level GPIO driver implementation for rp2040
 *
 * @author      Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * @}
 */

#include <stdint.h>

#include "assert.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    uint32_t idx_pin;
    uint32_t reg_ctrl;
    uint32_t reg_pad_ctrl;

    assert(((pin >> 8) == GPIO_BANK_USER));

    resets_hw->reset &= ~RESETS_RESET_IO_BANK0_BITS;
    resets_hw->reset &= ~RESETS_RESET_PADS_BANK0_BITS;

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_IO_BANK0_BITS)) {
        ;
    }

    while (!(resets_hw->reset_done & RESETS_RESET_DONE_PADS_BANK0_BITS)) {
        ;
    }

    idx_pin = (pin & 0x00ff);

    reg_ctrl = iobank0_hw->io[idx_pin].ctrl;
    reg_pad_ctrl = padsbank0_hw->io[idx_pin];

    // Set pin function to SIO.
    reg_ctrl &= ~IO_BANK0_GPIO0_CTRL_FUNCSEL_BITS;
    reg_ctrl |= (IO_BANK0_GPIO0_CTRL_FUNCSEL_VALUE_SIO_0 << IO_BANK0_GPIO0_CTRL_FUNCSEL_LSB);

    if (mode == GPIO_IN) {
        reg_pad_ctrl |= (1 << PADS_BANK0_GPIO0_OD_LSB);
        reg_pad_ctrl |= (1 << PADS_BANK0_GPIO0_IE_LSB);

        reg_ctrl &= ~IO_BANK0_GPIO0_CTRL_OEOVER_BITS;
        reg_ctrl |= (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_DISABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);

    }
    else if (mode == GPIO_OUT) {
        reg_pad_ctrl &= ~PADS_BANK0_GPIO0_OD_BITS;
        reg_pad_ctrl &= ~PADS_BANK0_GPIO0_IE_BITS;

        reg_ctrl &= ~IO_BANK0_GPIO0_CTRL_OEOVER_BITS;
        reg_ctrl |= (IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_ENABLE << IO_BANK0_GPIO0_CTRL_OEOVER_LSB);
    }
    else {
        return -1;
    }

    iobank0_hw->io[idx_pin].ctrl = reg_ctrl;
    padsbank0_hw->io[idx_pin] = reg_pad_ctrl;

    return 0;
}

/*
int gpio_read(gpio_t pin)
{
    return 0;
}*/

void gpio_set(gpio_t pin)
{
    uint32_t idx_pin;
    uint32_t reg_ctrl;

    assert(((pin >> 8) == GPIO_BANK_USER));

    idx_pin = (pin & 0x00ff);

    reg_ctrl = iobank0_hw->io[idx_pin].ctrl;

    if (((reg_ctrl & IO_BANK0_GPIO0_CTRL_OEOVER_BITS) >> IO_BANK0_GPIO0_CTRL_OEOVER_LSB) == IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_ENABLE) {
        reg_ctrl &= ~(IO_BANK0_GPIO0_CTRL_OUTOVER_BITS);
        reg_ctrl |= (IO_BANK0_GPIO0_CTRL_OUTOVER_VALUE_HIGH << IO_BANK0_GPIO0_CTRL_OUTOVER_LSB);

        iobank0_hw->io[idx_pin].ctrl = reg_ctrl;
    }
}

void gpio_clear(gpio_t pin)
{
    uint32_t idx_pin;
    uint32_t reg_ctrl;

    assert(((pin >> 8) == GPIO_BANK_USER));

    idx_pin = (pin & 0x00ff);

    reg_ctrl = iobank0_hw->io[idx_pin].ctrl;

    if (((reg_ctrl & IO_BANK0_GPIO0_CTRL_OEOVER_BITS) >> IO_BANK0_GPIO0_CTRL_OEOVER_LSB) == IO_BANK0_GPIO0_CTRL_OEOVER_VALUE_ENABLE) {
        reg_ctrl &= ~(IO_BANK0_GPIO0_CTRL_OUTOVER_BITS);
        reg_ctrl |= (IO_BANK0_GPIO0_CTRL_OUTOVER_VALUE_LOW << IO_BANK0_GPIO0_CTRL_OUTOVER_LSB);

        iobank0_hw->io[idx_pin].ctrl = reg_ctrl;
    }
}

/*
void gpio_toggle(gpio_t dev)
{

}*/

/*
void gpio_write(gpio_t dev, int value)
{

}*/
