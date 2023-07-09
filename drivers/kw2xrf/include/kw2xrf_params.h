/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_kw2xrf
 *
 * @{
 * @file
 * @brief       Default configuration for the KW2XRF driver
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 */

#ifndef KW2XRF_PARAMS_H
#define KW2XRF_PARAMS_H

#include "board.h"
#include "kw2xrf.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the KW2XRF driver
 * @{
 */
#ifndef KW2XRF_PARAM_SPI
#define KW2XRF_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef KW2XRF_PARAM_SPI_CLK
#define KW2XRF_PARAM_SPI_CLK     MHZ(9)
#endif
#ifndef KW2XRF_PARAM_CS
#define KW2XRF_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef KW2XRF_PARAM_INT
#define KW2XRF_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef KW2XRF_PARAM_SLEEP
#define KW2XRF_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef KW2XRF_PARAM_RESET
#define KW2XRF_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#ifndef KW2XRF_PARAMS
#define KW2XRF_PARAMS            { .spi = KW2XRF_PARAM_SPI,         \
                                   .spi_freq = KW2XRF_PARAM_SPI_CLK, \
                                   .cs_pin = KW2XRF_PARAM_CS,       \
                                   .int_pin = KW2XRF_PARAM_INT, \
                                   .rst_pin = KW2XRF_PARAM_RESET }
#endif
/**@}*/

/**
 * @brief   KW2XRF configuration
 */
static const kw2xrf_params_t kw2xrf_params[] =
{
    KW2XRF_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* KW2XRF_PARAMS_H */
/** @} */
