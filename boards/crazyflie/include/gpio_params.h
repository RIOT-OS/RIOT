/*
 * Copyright (C) 2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_crazyflie
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Crazyflie 2.1
 *
 * @author      LeonardHerbst <leonard.herbst@tu-dresden.de>
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
