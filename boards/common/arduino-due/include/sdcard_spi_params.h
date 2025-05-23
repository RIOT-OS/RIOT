/*
 * Copyright (C) 2017 Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 *               2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_common_arduino_due
 * @{
 *
 * @file
 * @brief       SD card configuration for the Arduino SD card shields
 *
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration for the sdcard_spi driver
 * @{
 */
#ifndef SDCARD_SPI_PARAM_SPI
#define SDCARD_SPI_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef SDCARD_SPI_PARAM_CS
#define SDCARD_SPI_PARAM_CS          (ARDUINO_PIN_4)
#endif
#ifndef SDCARD_SPI_PARAM_CLK
#define SDCARD_SPI_PARAM_CLK         (ARDUINO_PIN_76)
#endif
#ifndef SDCARD_SPI_PARAM_MOSI
#define SDCARD_SPI_PARAM_MOSI        (ARDUINO_PIN_75)
#endif
#ifndef SDCARD_SPI_PARAM_MISO
#define SDCARD_SPI_PARAM_MISO        (ARDUINO_PIN_74)
#endif
#ifndef SDCARD_SPI_PARAM_POWER
#define SDCARD_SPI_PARAM_POWER       (GPIO_UNDEF)
#endif
#ifndef SDCARD_SPI_PARAM_POWER_AH
/** treated as 'don't care' if SDCARD_SPI_PARAM_POWER is GPIO_UNDEF */
#define SDCARD_SPI_PARAM_POWER_AH    (true)
#endif
/** @} */

/**
 * @brief   sdcard_spi configuration
 */
static const  sdcard_spi_params_t sdcard_spi_params[] = {
    {
        .spi_dev        = SDCARD_SPI_PARAM_SPI,
        .cs             = SDCARD_SPI_PARAM_CS,
        .clk            = SDCARD_SPI_PARAM_CLK,
        .mosi           = SDCARD_SPI_PARAM_MOSI,
        .miso           = SDCARD_SPI_PARAM_MISO,
        .power          = SDCARD_SPI_PARAM_POWER,
        .power_act_high = SDCARD_SPI_PARAM_POWER_AH
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
