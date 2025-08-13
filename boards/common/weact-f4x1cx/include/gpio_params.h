/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_weact-f4x1cx
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
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
        .name = "LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    },
    {
        .name = "KEY",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
