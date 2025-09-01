/*
 * SPDX-FileCopyrightText: 2017 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mbed_lpc1768
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
        .name = "LED 2",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED 3",
        .pin = LED3_PIN,
        .mode = GPIO_OUT
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
