/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 *
 * @{
 * @file
 * @brief       Default configuration for the AT86RF2xx driver
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef AT86RF2XX_PARAMS_H
#define AT86RF2XX_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default configuration parameters for the AT86RF2xx driver
 * @{
 */
#ifndef AT86RF2XX_PARAM_SPI
#define AT86RF2XX_PARAM_SPI         (SPI_0)
#endif
#ifndef AT86RF2XX_PARAM_SPI_SPEED
#define AT86RF2XX_PARAM_SPI_SPEED   (SPI_SPEED_5MHZ)
#endif
#ifndef AT86RF2XX_PARAM_CS
#define AT86RF2XX_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef AT86RF2XX_PARAM_INT
#define AT86RF2XX_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef AT86RF2XX_PARAM_SLEEP
#define AT86RF2XX_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef AT86RF2XX_PARAM_RESET
#define AT86RF2XX_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#define AT86RF2XX_PARAMS_DEFAULT    {.spi = AT86RF2XX_PARAM_SPI, \
                                     .spi_speed = AT86RF2XX_PARAM_SPI_SPEED, \
                                     .cs_pin = AT86RF2XX_PARAM_CS, \
                                     .int_pin = AT86RF2XX_PARAM_INT, \
                                     .sleep_pin = AT86RF2XX_PARAM_SLEEP, \
                                     .reset_pin = AT86RF2XX_PARAM_RESET}
/**@}*/

/**
 * @brief   AT86RF231 configuration
 */
static const at86rf2xx_params_t at86rf2xx_params[] =
{
#ifdef AT86RF2XX_PARAMS_BOARD
    AT86RF2XX_PARAMS_BOARD,
#else
    AT86RF2XX_PARAMS_DEFAULT,
#endif
};

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_PARAMS_H */
/** @} */
