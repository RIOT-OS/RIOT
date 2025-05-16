/*
 * Copyright (C) 2017 Neo Nenaco <neo@nenaco.de>
 * Copyright (C) 2017 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mrf24j40
 *
 * @{
 * @file
 * @brief       Default configuration for the MRF24J40 driver
 *
 * @author      Neo Nenaco <neo@nenaco.de>
 * @author      Koen Zandberg <koen@bergzand.net>
 */


#include "board.h"
#include "mrf24j40.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the MRF24J40 driver
 * @{
 */
#ifndef MRF24J40_PARAM_SPI
#define MRF24J40_PARAM_SPI          (SPI_DEV(0))
#endif
#ifndef MRF24J40_PARAM_SPI_CLK
#define MRF24J40_PARAM_SPI_CLK      (SPI_CLK_5MHZ)
#endif
#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS           (GPIO_PIN(0, 0))
#endif
#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT          (GPIO_PIN(0, 1))
#endif
#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET        (GPIO_PIN(0, 3))
#endif

#ifndef MRF24J40_PARAMS
#define MRF24J40_PARAMS             { .spi = MRF24J40_PARAM_SPI,         \
                                      .spi_clk = MRF24J40_PARAM_SPI_CLK, \
                                      .cs_pin = MRF24J40_PARAM_CS,       \
                                      .int_pin = MRF24J40_PARAM_INT,     \
                                      .reset_pin = MRF24J40_PARAM_RESET }
#endif
/**@}*/

/**
 * @brief   MRF24J40 configuration
 */
static const mrf24j40_params_t mrf24j40_params[] =
{
    MRF24J40_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
