/*
 * SPDX-FileCopyrightText: 2019 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_at86rf215
 * @{
 *
 * @file
 * @brief       Default configuration for the AT86RF215 driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "at86rf215.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT86RF215 driver
 *          Example config for EXT3 on same54-xpro
 * @{
 */
#ifndef AT86RF215_PARAM_SPI
#define AT86RF215_PARAM_SPI         (SPI_DEV(1))
#endif
#ifndef AT86RF215_PARAM_SPI_CLK
#define AT86RF215_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif
#ifndef AT86RF215_PARAM_CS
#define AT86RF215_PARAM_CS          (GPIO_PIN(2, 14))
#endif
#ifndef AT86RF215_PARAM_INT
#define AT86RF215_PARAM_INT         (GPIO_PIN(2, 30))
#endif
#ifndef AT86RF215_PARAM_RESET
#define AT86RF215_PARAM_RESET       (GPIO_PIN(3, 10))
#endif

#ifndef AT86RF215_PARAMS
#define AT86RF215_PARAMS            { .spi = AT86RF215_PARAM_SPI,         \
                                      .spi_clk = AT86RF215_PARAM_SPI_CLK, \
                                      .cs_pin = AT86RF215_PARAM_CS,       \
                                      .int_pin = AT86RF215_PARAM_INT,     \
                                      .reset_pin = AT86RF215_PARAM_RESET }
#endif
/**@}*/

/**
 * @brief   AT86RF215 configuration
 */
static const at86rf215_params_t at86rf215_params[] =
{
    AT86RF215_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
