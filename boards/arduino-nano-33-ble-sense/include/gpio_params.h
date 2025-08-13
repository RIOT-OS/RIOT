/*
 * SPDX-FileCopyrightText: 2020 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arduino-nano-33-ble-sense
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Méwen Berthelot <berthelotmewen@gmail.com>
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
        .name  = "LED0 (Orange)",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
    },
    {
        .name  = "LED1 (RED)",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED2 (GREEN)",
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED3 (BLUE)",
        .pin   = LED3_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED4 (PWR)",
        .pin   = LED4_PIN,
        .mode  = GPIO_OUT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
