/*
 * Copyright (C) 2015 Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   rf-motherboard
 * @{
 *
 * @file
 * @brief     at86rf212b board specific configuration
 *
 * @author    Nick van IJzendoorn <nijzendoorn@engineering-spirit.nl>
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
            .spi = AT86RF212B_SPI,
            .spi_speed = AT86RF212B_SPI_CLK,
            .cs_pin = AT86RF212B_CS,
            .int_pin = AT86RF212B_INT,
            .sleep_pin = AT86RF212B_SLEEP,
            .reset_pin = AT86RF212B_RESET,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* AT86RF2XX_PARAMS_H_ */
/** @} */
