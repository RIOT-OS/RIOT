/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_bitcraze_crazyflie21_main
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Crazyflie 2.1 main application MCU
 *
 * @author      Leonard Herbst <leonard.herbst@tu-dresden.de>
 */

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
        .name = "red LED top right",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "green LED top left",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "blue LED bottom left",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "red LED top left",
        .pin = LED3_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "green LED top left",
        .pin = LED4_PIN,
        .mode = GPIO_OUT
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
