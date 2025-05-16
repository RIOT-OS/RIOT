/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_encx24j600
 * @{
 *
 * @file
 * @brief       Default configuration for the ENCX24J600 Ethernet driver
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */


#include "encx24j600.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ENCX24J600 driver
 * @{
 */
#ifndef ENCX24J600_PARAM_SPI
#define ENCX24J600_PARAM_SPI      (SPI_DEV(0))
#endif
#ifndef ENCX24J600_PARAM_CS
#define ENCX24J600_PARAM_CS       (GPIO_PIN(0, 0))
#endif
#ifndef ENCX24J600_PARAM_INT
#define ENCX24J600_PARAM_INT      (GPIO_PIN(0, 1))
#endif

#ifndef ENCX24J600_PARAMS
#define ENCX24J600_PARAMS         { .spi = ENCX24J600_PARAM_SPI,     \
                                    .cs_pin = ENCX24J600_PARAM_CS,   \
                                    .int_pin = ENCX24J600_PARAM_INT }
#endif
/** @} */

/**
 * @brief   ENCX24J600 configuration
 */
static const  encx24j600_params_t encx24j600_params[] = {
    ENCX24J600_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
