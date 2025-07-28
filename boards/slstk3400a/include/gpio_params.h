/*
 * SPDX-FileCopyrightText: 2016-2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_slstk3400a
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
        .name = "LED 0",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED 1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button 1",
        .pin = PB0_PIN,
        .mode = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "Button 2",
        .pin = PB1_PIN,
        .mode = GPIO_IN_PU,
        .flags = SAUL_GPIO_INVERTED
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
