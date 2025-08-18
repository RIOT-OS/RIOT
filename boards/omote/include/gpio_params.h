/*
 * SPDX-FileCopyrightText: 2020 Oppila Microsystems -  http://www.oppila.in
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   boards_omote
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
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
        .name  = "LED(green1)",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = 0x0,
    },
    {
        .name  = "LED(green2)",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = 0x0,
    },
    {
        .name  = "Button(User)",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
