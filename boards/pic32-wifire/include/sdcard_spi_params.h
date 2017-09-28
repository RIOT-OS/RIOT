/*
 * Copyright (C) 2017 Imagination Technologies
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pic32-wifire
 * @{
 *
 * @file
 * @brief       SD card configuration for the Wifire
 *
 * @author      Neil Jones
 */

#ifndef SDCARD_SPI_PARAMS_H
#define SDCARD_SPI_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default configuration for the sdcard_spi driver
 * @{
 */
#ifndef SDCARD_SPI_PARAM_SPI
#define SDCARD_SPI_PARAM_SPI         (SPI_DEV(3))
#endif
#ifndef SDCARD_SPI_PARAM_CS
#define SDCARD_SPI_PARAM_CS          (GPIO_PIN(PORT_C, 3))
#endif
#ifndef SDCARD_SPI_PARAM_CLK
#define SDCARD_SPI_PARAM_CLK         (GPIO_PIN(PORT_B, 14))
#endif
#ifndef SDCARD_SPI_PARAM_MOSI
#define SDCARD_SPI_PARAM_MOSI        (GPIO_PIN(PORT_C, 4))
#endif
#ifndef SDCARD_SPI_PARAM_MISO
#define SDCARD_SPI_PARAM_MISO        (GPIO_PIN(PORT_B, 10))
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

#endif /* SDCARD_SPI_PARAMS_H */
/** @} */
