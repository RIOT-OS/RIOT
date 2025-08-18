/*
 * SPDX-FileCopyrightText: 2021 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_atxmega-a1-xplained
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GPIO configuration
 */
static const saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "SW0",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW1",
        .pin = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW2",
        .pin = BTN2_PIN,
        .mode = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW3",
        .pin = BTN3_PIN,
        .mode = BTN3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW4",
        .pin = BTN4_PIN,
        .mode = BTN4_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW5",
        .pin = BTN5_PIN,
        .mode = BTN5_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW6",
        .pin = BTN6_PIN,
        .mode = BTN6_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "SW7",
        .pin = BTN7_PIN,
        .mode = BTN7_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED0",
        .pin = LED0_PIN,
        .mode = LED0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED1",
        .pin = LED1_PIN,
        .mode = LED1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED2",
        .pin = LED2_PIN,
        .mode = LED2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED3",
        .pin = LED3_PIN,
        .mode = LED3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED4",
        .pin = LED4_PIN,
        .mode = LED4_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED5",
        .pin = LED5_PIN,
        .mode = LED5_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED6",
        .pin = LED6_PIN,
        .mode = LED6_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED7",
        .pin = LED7_PIN,
        .mode = LED7_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
