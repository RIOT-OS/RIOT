/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_esp8266_esp-12x
 * @brief       Board specific configuration of direct mapped GPIOs
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "LED",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR)
    }
};

#ifdef __cplusplus
}
#endif

/** @} */
