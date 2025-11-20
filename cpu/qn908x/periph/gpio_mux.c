/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_qn908x
 *
 * @{
 *
 * @file
 * @brief       Common Pin MUX functions.
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "gpio_mux.h"

#include "vendor/drivers/fsl_iocon.h"

void gpio_init_mux(gpio_t pin, uint32_t func)
{
    if (pin == GPIO_UNDEF) {
        return;
    }
    IOCON_PinMuxSet(IOCON, GPIO_T_PORT(pin), GPIO_T_PIN(pin), func);
}
