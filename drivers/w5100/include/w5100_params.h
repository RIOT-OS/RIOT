/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_w5100
 * @{
 *
 * @file
 * @brief       Default parameters for W5100 Ethernet devices
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "board.h"

#ifdef MODULE_SHIELD_W5100
#include "arduino_iomap.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MODULE_SHIELD_W5100
#define W5100_PARAM_SPI         ARDUINO_SPI_ISP
#define W5100_PARAM_CS          ARDUINO_PIN_10
#define W5100_PARAM_EVT         ARDUINO_PIN_2
#endif

/**
 * @name    Default configuration parameters for the W5100 driver
 * @{
 */
#ifndef W5100_PARAM_SPI
#define W5100_PARAM_SPI         (SPI_DEV(0))       /**< Default SPI device */
#endif
#ifndef W5100_PARAM_SPI_CLK
#define W5100_PARAM_SPI_CLK     (SPI_CLK_5MHZ)     /**< Default SPI speed */
#endif
#ifndef W5100_PARAM_CS
#define W5100_PARAM_CS          (GPIO_PIN(0, 0))   /**< Default SPI chip select pin */
#endif
#ifndef W5100_PARAM_EVT
#define W5100_PARAM_EVT         (GPIO_PIN(0, 1))   /**< Default event pin */
#endif

#ifndef W5100_PARAMS
#define W5100_PARAMS            { .spi = W5100_PARAM_SPI,     \
                                  .clk = W5100_PARAM_SPI_CLK, \
                                  .cs  = W5100_PARAM_CS,      \
                                  .evt = W5100_PARAM_EVT }
#endif
/** @} */

/**
 * @brief   W5100 configuration
 */
static const  w5100_params_t w5100_params[] = {
    W5100_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
