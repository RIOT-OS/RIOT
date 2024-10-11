/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waveshare_nrf52840_eval_kit
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped GPIO pins
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

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
#endif /* GPIO_PARAMS_H */
