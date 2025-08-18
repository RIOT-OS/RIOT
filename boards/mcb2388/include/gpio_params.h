/*
 * SPDX-FileCopyrightText: 2019 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mcb2388
 * @{
 *
 * @file
 * @brief       Board specific configuration of direct mapped GPIOs
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
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
        .name = "LED0",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED1",
        .pin = LED1_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED2",
        .pin = LED2_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED3",
        .pin = LED3_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED4",
        .pin = LED4_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED5",
        .pin = LED5_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED6",
        .pin = LED6_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "LED7",
        .pin = LED7_PIN,
        .mode = GPIO_OUT,
    },
    {
        .name = "Button(INT0)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
