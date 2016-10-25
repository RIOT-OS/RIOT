/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_kw2xrf
 * @brief       generic kw2xrf pin config
 *
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KW2XRF_PARAMS_H
#define KW2XRF_PARAMS_H

/**
 * @brief make sure the SPI port and the needed GPIO pins are defined
 * @{
 */
#ifndef KWRF_SPI
#error "SPI not defined"
#endif
#ifndef KWRF_CS
#error "Chip select pin not defined"
#endif
#ifndef KWRF_INT
#error "Interrupt pin not defined"
#endif
#ifndef KWRF_SPI_SPEED
#define KWRF_SPI_SPEED          (SPI_SPEED_10MHZ)
#endif
/**@}*/

/**
 * @name KW2XRF configuration
 */
static const  kw2xrf_params_t kw2xrf_params[] =
    {
        {
            .spi = KWRF_SPI,
            .spi_speed = KWRF_SPI_SPEED,
            .cs_pin = KWRF_CS,
            .int_pin = KWRF_INT,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* KW2XRF_PARAMS_H */
/** @} */
