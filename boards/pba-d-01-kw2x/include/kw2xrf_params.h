/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_pba-d-01-kw2x
 * @{
 *
 * @file
 * @brief     kw2xrf board specific configuration
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 * @author    Jonas Remmert <j.remmert@phytec.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef KW2XRF_PARAMS_H
#define KW2XRF_PARAMS_H

/**
 * @name KW2XRF configuration
 */
static const kw2xrf_params_t kw2xrf_params[] =
    {
        {
            .spi = KW2XRF_SPI,
            .spi_speed = KW2XRF_SPI_SPEED,
            .cs_pin = KW2XRF_CS,
            .int_pin = KW2XRF_INT,
        },
    };
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* KW2XRF_PARAMS_H */
/** @} */
