/*
 * Copyright (C) 2016 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup   boards_remote-reva
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped GPIOs
 *
 * @author    Sebastian Meiling <s@mlng.net>
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
        .name  = "LED(red)",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = 0x0,
    },
    {
        .name  = "LED(green)",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = 0x0,
    },
    {
        .name  = "LED(blue)",
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = 0x0,
    },
    {
        .name  = "Button(User)",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
