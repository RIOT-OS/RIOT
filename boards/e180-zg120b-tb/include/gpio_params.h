/*
 * SPDX-FileCopyrightText: 2016-2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_e180-zg120b-tb
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
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
        .name = "LED0",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "PB0",
        .pin = PB0_PIN,
        .mode = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "PB1",
        .pin = PB1_PIN,
        .mode = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "PB2",
        .pin = PB2_PIN,
        .mode = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
