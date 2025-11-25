/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
