/*
 * SPDX-FileCopyrightText: 2017 Kees Bakker, SODAQ
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_sodaq-one
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Kees Bakker <kees@sodaq.com>
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
        .name = "LED Red",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED Green",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED Blue",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
