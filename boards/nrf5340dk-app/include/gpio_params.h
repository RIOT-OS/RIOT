/*
 * SPDX-FileCopyrightText: 2026 Christian Amsüss <chrysn@fsfe.org>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup    boards_nrf5340dk-app
 * @{
 *
 * @file
 * @brief      Configuration of SAUL mapped GPIO pins
 *
 * @author     Christian Amsüss <chrysn@fsfe.org>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "Button 1",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name  = "Button 2",
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name  = "Button 2",
        .pin   = BTN2_PIN,
        .mode  = BTN2_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name  = "Button 4",
        .pin   = BTN3_PIN,
        .mode  = BTN3_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name  = "LED1",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED2",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED3",
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED4",
        .pin   = LED3_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
