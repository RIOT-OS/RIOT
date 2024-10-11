/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sipeed_longan_nano
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
 * @brief    LED/Button SAUL configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name  = "BOOT",
        .pin   = BTN0_PIN,
        .mode  = BTN0_MODE,
    },
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
};

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* GPIO_PARAMS_H */
