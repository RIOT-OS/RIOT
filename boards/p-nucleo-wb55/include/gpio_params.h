/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_p-nucleo-wb55
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
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
        .name = "LED(blue)",
        .pin = LED0_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED(green)",
        .pin = LED1_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "LED(red)",
        .pin = LED2_PIN,
        .mode = GPIO_OUT
    },
    {
        .name = "Button(B1 User)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Button(B2 User)",
        .pin  = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "Button(B3 User)",
        .pin  = BTN2_PIN,
        .mode = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
