/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_elecrow-crowpanel-esp32s3-rotary-128
 * @brief       Board specific configuration of direct mapped GPIOs for the
 *              Elecrow CrowPanel ESP32-S3 1.28-inch rotary round display
 *              board (Touch version)
 * @file
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 * @{
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Boot button configuration
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
        .name = "DISPLAY_BTN",
        .pin = BTN1_PIN,
        .mode = BTN1_MODE,
        .flags = SAUL_GPIO_INVERTED
    },
    {
        .name = "LED Red",
        .pin = LED0_PIN,
        .mode = GPIO_OUT,
        .flags = (SAUL_GPIO_INVERTED | SAUL_GPIO_INIT_CLEAR),
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
