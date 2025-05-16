/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_enc28j60
 * @{
 *
 * @file
 * @brief       Default configuration for the ENC28J60 driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */


#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the ENC28J60 driver
 * @{
 */
#ifndef ENC28J60_PARAM_SPI
#define ENC28J60_PARAM_SPI      (SPI_DEV(0))
#endif
#ifndef ENC28J60_PARAM_CS
#define ENC28J60_PARAM_CS       (GPIO_PIN(0, 0))
#endif
#ifndef ENC28J60_PARAM_INT
#define ENC28J60_PARAM_INT      (GPIO_PIN(0, 1))
#endif
#ifndef ENC28J60_PARAM_RESET
#define ENC28J60_PARAM_RESET    (GPIO_PIN(0, 2))
#endif

#ifndef ENC28J60_PARAMS
#define ENC28J60_PARAMS         { .spi = ENC28J60_PARAM_SPI,     \
                                  .cs_pin = ENC28J60_PARAM_CS,   \
                                  .int_pin = ENC28J60_PARAM_INT, \
                                  .rst_pin = ENC28J60_PARAM_RESET }
#endif
/** @} */

/**
 * @brief   ENC28J60 configuration
 */
static const  enc28j60_params_t enc28j60_params[] = {
    ENC28J60_PARAMS
};

#ifdef __cplusplus
}
#endif

/** @} */
