/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_feather-m0
 * @{
 *
 * @file
 * @brief       SD card configuration for the Adafruit Feather M0 Adalogger
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Card detect pin
 */
 #define CARD_DETECT_PIN              (GPIO_PIN(PA, 21))

/**
 * @brief   sdcard_spi configuration
 */
static const  sdcard_spi_params_t sdcard_spi_params[] = {
    {
        .spi_dev        = SPI_DEV(0),
        .cs             = GPIO_PIN(PA, 8),
        .clk            = GPIO_PIN(PB, 11),
        .mosi           = GPIO_PIN(PB, 10),
        .miso           = GPIO_PIN(PA, 12),
        .power          = GPIO_UNDEF,
        .power_act_high = true
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
