/*
  * Copyright (C) 2016 Hochschule für Angewandte Wissenschaften Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mrf24j40
 *
 * @{
 * @file
 * @brief       Default configuration for the MRF24J40 driver
 *
 */



#ifndef MRF24J40_PARAMS_H
#define MRF24J40_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the MRF24J40 driver
 * @{
 */
#ifndef MRF24J40_PARAM_SPI
#define MRF24J40_PARAM_SPI         (SPI_0)
#endif
#ifndef MRF24J40_PARAM_SPI_SPEED
#define MRF24J40_PARAM_SPI_SPEED   (SPI_SPEED_5MHZ)
#endif
#ifndef MRF24J40_PARAM_CS
#define MRF24J40_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef MRF24J40_PARAM_INT
#define MRF24J40_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef MRF24J40_PARAM_SLEEP
#define MRF24J40_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef MRF24J40_PARAM_RESET
#define MRF24J40_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#define MRF24J40_PARAMS_DEFAULT    {.spi = MRF24J40_PARAM_SPI, \
                                     .spi_speed = MRF24J40_PARAM_SPI_SPEED, \
                                     .cs_pin = MRF24J40_PARAM_CS, \
                                     .int_pin = MRF24J40_PARAM_INT, \
                                     .sleep_pin = MRF24J40_PARAM_SLEEP, \
                                     .reset_pin = MRF24J40_PARAM_RESET}
/**@}*/

/**
 * @brief   MRF24J40 configuration
 */
static const mrf24j40_params_t mrf24j40_params[] =
{
#ifdef MRF24J40_PARAMS_BOARD
    MRF24J40_PARAMS_BOARD,
#else
    MRF24J40_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* MRF24J40_PARAMS_H */
/** @} */
