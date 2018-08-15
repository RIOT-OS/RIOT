/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sensebox_samd21
 * @{
 *
 * @file
 * @brief       SD card configuration for SenseBox MCU with SAMD21 board. This
 * configuration
 *              considers that SD is connected to the XBEE2 port.
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef SDCARD_SPI_PARAMS_H
#define SDCARD_SPI_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Card detect pin
 */
 #define CARD_DETECT_PIN              (XBEE2_INT_PIN)

/**
 * @brief   sdcard_spi configuration
 */
static const  sdcard_spi_params_t sdcard_spi_params[] = {
    {
        .spi_dev        = SPI_DEV(0),
        .cs             = XBEE2_CS_PIN,
        .clk            = GPIO_PIN(PA, 17),
        .mosi           = GPIO_PIN(PA, 16),
        .miso           = GPIO_PIN(PA, 19),
        .power          = GPIO_UNDEF,
        .power_act_high = true
    },
};

#ifdef __cplusplus
}
#endif

#endif /* SDCARD_SPI_PARAMS_H */
/** @} */
