/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_adafruit_metro_esp32s3
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
 * @brief   LED and Button configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "BOOT",
        .pin = BTN0_PIN,
        .mode = BTN0_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "LED red",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = SAUL_GPIO_INIT_CLEAR
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
