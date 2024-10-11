/*
 * Copyright (C) 2019 Inria
 *               2019 Freie Universität Berlin
 *               2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_pyboard
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LD1",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button(B1 User)",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
