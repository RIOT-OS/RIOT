/*
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2016 UC Berkeley
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_hamilton
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @author    Sebastian Meiling <s@mlng.net>
 * @author    Michael Andersen <m.andersen@berkeley.edu>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED(red)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Button",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
