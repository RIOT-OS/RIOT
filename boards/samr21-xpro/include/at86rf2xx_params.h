/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_samr21-xpro
 * @{
 *
 * @file
 * @brief     at86rf233 board specific configuration
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef AT86RF2XX_PARAMS_H_
#define AT86RF2XX_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name AT86RF231 configuration
 */
static const  at86rf2xx_params_t at86rf2xx_params[] =
    {
        {
            .spi = AT86RF233_SPI,
            .spi_speed = AT86RF233_SPI_CLK,
            .cs_pin = AT86RF233_CS,
            .int_pin = AT86RF233_INT,
            .sleep_pin = AT86RF233_SLEEP,
            .reset_pin = AT86RF233_RESET,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* AT86RF2XX_PARAMS_H_ */
/** @} */
