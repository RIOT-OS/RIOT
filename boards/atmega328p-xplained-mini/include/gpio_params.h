/*
 * SPDX-FileCopyrightText: 2023 Gerson Fernando Budke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_atmega328p_xplained_mini
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
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "BTN0 (SW0)",
        .pin  = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name = "LED0 (Yellow)",
        .pin = LED0_PIN,
        .mode = LED0_MODE,
        .flags = (SAUL_GPIO_INIT_CLEAR),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
