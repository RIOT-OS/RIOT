/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_mulle
 * @{
 *
 * @file
 * @brief     at86rf231 board specific configuration
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @author    Joakim Gebart <joakim.gebart@eistec.se>
 */

#ifndef AT86RF2XX_PARAMS_H_
#define AT86RF2XX_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name AT86RF212B configuration
 */
static const  at86rf2xx_params_t at86rf2xx_params[] =
    {
        {
            .spi = AT86RF231_SPI,
            .spi_speed = AT86RF231_SPI_CLK,
            .cs_pin = AT86RF231_CS,
            .int_pin = AT86RF231_INT,
            .sleep_pin = AT86RF231_SLEEP,
            .reset_pin = AT86RF231_RESET,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* AT86RF2XX_PARAMS_H */
/** @} */
