/*
 * Copyright (C) 2016-2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_stk3600
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
