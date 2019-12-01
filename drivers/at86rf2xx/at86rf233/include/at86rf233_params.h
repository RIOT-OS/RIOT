/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       Default configuration for a AT86RF233 device
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF233_PARAMS_H
#define AT86RF233_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT86RF233 driver
 * @{
 */
#ifndef AT86RF233_PARAM_SPI
#define AT86RF233_PARAM_SPI         (SPI_DEV(0))
#endif
#ifndef AT86RF233_PARAM_SPI_CLK
#define AT86RF233_PARAM_SPI_CLK     (SPI_CLK_5MHZ)
#endif
#ifndef AT86RF233_PARAM_CS
#define AT86RF233_PARAM_CS          (GPIO_PIN(0, 0))
#endif
#ifndef AT86RF233_PARAM_INT
#define AT86RF233_PARAM_INT         (GPIO_PIN(0, 1))
#endif
#ifndef AT86RF233_PARAM_SLEEP
#define AT86RF233_PARAM_SLEEP       (GPIO_PIN(0, 2))
#endif
#ifndef AT86RF233_PARAM_RESET
#define AT86RF233_PARAM_RESET       (GPIO_PIN(0, 3))
#endif

#ifndef AT86RF233_PARAMS
#define AT86RF233_PARAMS            { .base_params = {                        \
                                          .spi = AT86RF233_PARAM_SPI,          \
                                          .spi_clk = AT86RF233_PARAM_SPI_CLK,  \
                                          .cs_pin = AT86RF233_PARAM_CS,        \
                                          .int_pin = AT86RF233_PARAM_INT,      \
                                          .sleep_pin = AT86RF233_PARAM_SLEEP,  \
                                          .reset_pin = AT86RF233_PARAM_RESET } }
#endif
/**@}*/

/**
 * @brief   Array of configuration parameters for AT86RF233 transceivers
 */
static const at86rf233_params_t at86rf233_params[] =
{
    AT86RF233_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* AT86RF233_PARAMS_H */
/** @} */
