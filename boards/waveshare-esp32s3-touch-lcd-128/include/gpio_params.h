/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_waveshare-esp32s3-touch-lcd-128
 * @brief       Board specific configuration of direct mapped GPIOs for the
 *              Waveshare ESP32-S3 1.28-inch round display board (Touch
 *              version)
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
};

#ifdef __cplusplus
}
#endif

/** @} */
