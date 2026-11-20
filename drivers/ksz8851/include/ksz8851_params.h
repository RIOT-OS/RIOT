/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_ksz8851
 * @{
 *
 * @file
 * @brief       Default parameters for the KSZ8851SNL driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the KSZ8851SNL driver
 *          Example config for EXT3 on same54-xpro
 * @{
 */
#ifndef KSZ8851_PARAM_SPI
/** @brief SPI device used to connect to the KSZ8851SNL */
#  define KSZ8851_PARAM_SPI          (SPI_DEV(1))
#endif
#ifndef KSZ8851_PARAM_SPI_CLK
/** @brief SPI clock speed used to communicate with the KSZ8851SNL */
#  define KSZ8851_PARAM_SPI_CLK      (SPI_CLK_10MHZ)
#endif
#ifndef KSZ8851_PARAM_CS
/** @brief GPIO pin connected to the KSZ8851SNL chip select */
#  define KSZ8851_PARAM_CS           (GPIO_PIN(2, 14))
#endif
#ifndef KSZ8851_PARAM_INT
/** @brief GPIO pin connected to the KSZ8851SNL interrupt output */
#  define KSZ8851_PARAM_INT          (GPIO_PIN(2, 30))
#endif
#ifndef KSZ8851_PARAM_RESET
/** @brief GPIO pin connected to the KSZ8851SNL reset input */
#  define KSZ8851_PARAM_RESET        (GPIO_PIN(3, 10))
#endif

#ifndef KSZ8851_PARAMS
/** @brief Default initializer for a KSZ8851SNL parameter structure */
#  define KSZ8851_PARAMS             { .spi   = KSZ8851_PARAM_SPI,     \
                                      .clk   = KSZ8851_PARAM_SPI_CLK, \
                                      .cs    = KSZ8851_PARAM_CS,      \
                                      .irq   = KSZ8851_PARAM_INT,     \
                                      .reset = KSZ8851_PARAM_RESET }
#endif
/** @} */

/**
 * @brief   KSZ8851SNL configuration
 */
static const ksz8851_params_t ksz8851_params[] = { KSZ8851_PARAMS };

#ifdef __cplusplus
}
#endif

/** @} */
