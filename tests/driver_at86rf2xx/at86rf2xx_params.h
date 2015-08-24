/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_at86rf2xx
 * @brief       generic at86rf231 pin config
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef AT86RF2XX_PARAMS_H
#define AT86RF2XX_PARAMS_H

/**
 * @brief make sure the SPI port and the needed GPIO pins are defined
 * @{
 */
#ifndef ATRF_SPI
#error "SPI not defined"
#endif
#ifndef ATRF_CS
#error "Chip select pin not defined"
#endif
#ifndef ATRF_INT
#error "Interrupt pin not defined"
#endif
#ifndef ATRF_SLEEP
#error "Sleep pin not defined"
#endif
#ifndef ATRF_RESET
#error "Reset pin not defined"
#endif
#ifndef ATRF_SPI_SPEED
#define ATRF_SPI_SPEED          (SPI_SPEED_5MHZ)
#endif
/**@}*/

/**
 * @name AT86RF231 configuration
 */
static const  at86rf2xx_params_t at86rf2xx_params[] =
    {
        {
            .spi = ATRF_SPI,
            .spi_speed = ATRF_SPI_SPEED,
            .cs_pin = ATRF_CS,
            .int_pin = ATRF_INT,
            .sleep_pin = ATRF_SLEEP,
            .reset_pin = ATRF_RESET,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* AT86RF2XX_PARAMS_H */
/** @} */
