/*
 * SPDX-FileCopyrightText: 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-FileCopyrightText: 2015 Hauke Petersen <hauke.petersen@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       Default configuration for the AT86RF2xx driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "at86rf2xx.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT86RF2xx driver
 * @{
 */
#ifndef AT86RF2XX_PARAM_SPI
/**
 * @brief   SPI bus the AT86RF2xx is connected to
 */
#define AT86RF2XX_PARAM_SPI         (SPI_DEV(0))
#endif

#ifndef AT86RF2XX_PARAM_SPI_CLK
/**
 * @brief   Clock to drive the SPI bus when speaking with the AT86RF2xx
 */
#define AT86RF2XX_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif

#ifndef AT86RF2XX_PARAM_CS
/**
 * @brief   GPIO pin the CS pin of the AT86RF2xx is connected to
 */
#define AT86RF2XX_PARAM_CS          (GPIO_PIN(0, 0))
#endif

#ifndef AT86RF2XX_PARAM_INT
/**
 * @brief   GPIO pin the INT pin of the AT86RF2xx is connected to
 */
#define AT86RF2XX_PARAM_INT         (GPIO_PIN(0, 1))
#endif

#ifndef AT86RF2XX_PARAM_SLEEP
/**
 * @brief   GPIO pin the SLEEP pin of the AT86RF2xx is connected to
 */
#define AT86RF2XX_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif

#ifndef AT86RF2XX_PARAM_RESET
/**
 * @brief   GPIO pin the RESET pin of the AT86RF2xx is connected to
 */
#define AT86RF2XX_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#ifndef AT86RF2XX_PARAMS
/**
 * @brief   Parameters to initialize the AT86RF2xx driver with
 */
#define AT86RF2XX_PARAMS            { .spi = AT86RF2XX_PARAM_SPI,         \
                                      .spi_clk = AT86RF2XX_PARAM_SPI_CLK, \
                                      .cs_pin = AT86RF2XX_PARAM_CS,       \
                                      .int_pin = AT86RF2XX_PARAM_INT,     \
                                      .sleep_pin = AT86RF2XX_PARAM_SLEEP, \
                                      .reset_pin = AT86RF2XX_PARAM_RESET }
#endif
/**@}*/

/**
 * @brief   AT86RF231 configuration
 */
#if AT86RF2XX_IS_PERIPH
static const uint8_t at86rf2xx_params[] =
{
    0 /* dummy value */
};
#else
static const at86rf2xx_params_t at86rf2xx_params[] =
{
    AT86RF2XX_PARAMS
};
#endif

#ifdef __cplusplus
}
#endif

/** @} */
