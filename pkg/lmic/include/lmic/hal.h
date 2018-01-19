/*
 * Copyright (c) 2017 Inria Chile
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lmic
 * @brief       LMIC HAL definitions
 * @{
 *
 * @file
 * @brief       LMIC HAL definitions
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LMIC_HAL_H
#define LMIC_HAL_H

#include "periph/gpio.h"

#define DIO_NUMOF (3U)

/**
 * @brief   Configuration for radion device (either SX1276 or SX1272)
 */
typedef struct lmic_pinmap_t {
    gpio_t nss;                  /**< CS pin */
    gpio_t rxtx;                 /**< RXTX pin */
    gpio_t rst;                  /**< Reset pin */
    gpio_t dio[DIO_NUMOF];       /**< Interrupt pins */
} lmic_pinmap;

/* Declared here, to be defined and initialized by the application */
extern const lmic_pinmap lmic_pins;

#endif /* LMIC_HAL_H */
/** @} */
