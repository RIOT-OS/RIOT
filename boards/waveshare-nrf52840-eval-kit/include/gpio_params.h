/*
 * SPDX-FileCopyrightText: 2020 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
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
 * @brief    LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "LED 1",
        .pin   = LED0_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED 2",
        .pin   = LED1_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED 3",
        .pin   = LED2_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "LED 4",
        .pin   = LED3_PIN,
        .mode  = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
    {
        .name  = "KEY1",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
    {
        .name  = "RESET",
        .pin   = BTN1_PIN,
        .mode  = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
