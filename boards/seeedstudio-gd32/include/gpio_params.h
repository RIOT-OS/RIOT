/*
 * SPDX-FileCopyrightText: 2020 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_seeedstudio-gd32
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    LED/Button SAUL configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "LED RED",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED GREEN",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED BLUE",
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "KEY1",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
    },
    {
        .name  = "KEY2",
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
