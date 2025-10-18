/*
 * SPDX-FileCopyrightText: 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @addtogroup cpu_native
 * @{
 */

/**
 * @file
 * @brief  Native CPU peripheral configuration
 * @author Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 */

#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief System core clock in Hz
 *
 *  1GHz is an arbitrary value used for compatibility with other platforms.
 */
#if !defined(CLOCK_CORECLOCK) || defined(DOXYGEN)
#  define CLOCK_CORECLOCK     GHZ(1)
#endif

/**
 * @brief Hardware timer clock skew avoidance
 */
#define NATIVE_TIMER_MIN_RES 200

/**
 * @brief Random Number Generator configuration
 */
#define RANDOM_NUMOF        (1U)

/* MARK: - Timer peripheral configuration */
/**
 * @name Timer peripheral configuration
 * @{
 */
#define TIMER_NUMOF            (1U)
#define TIMER_CHANNEL_NUMOF    (1U)    /**< Number of timer channels */
/** @} */

/**
 * @brief UART configuration
 */
#if !defined(UART_NUMOF) || defined(DOXYGEN)
#  define UART_NUMOF (1U)
#endif

/**
 * @brief PWM configuration
 */
#if !defined(PWM_NUMOF) || defined(DOXYGEN)
#  define PWM_NUMOF (8U)
#endif

/**
 * @brief QDEC configuration
 */
#if !defined(QDEC_NUMOF) || defined(DOXYGEN)
#  define QDEC_NUMOF (8U)
#endif

/* MARK: - SPI configuration (Linux host only) */
/**
 * @name SPI configuration (Linux host only)
 * @{
 */
#if !defined(SPI_NUMOF) || defined(DOXYGEN)
/**
 * @brief Amount of SPI devices
 *
 * Allows up to SPI_NUMOF SPI devices with each having up to SPI_MAXCS hardware
 * cable select lines. Assignment to hardware devices can be configured at
 * runtime using the `--spi` startup parameter.
 *
 * Can be overridden during compile time with a `-DSPI_NUMOF=n` flag.
 */
#  define SPI_NUMOF (1U)
#endif

#if !defined(SPI_MAXCS) || defined(DOXYGEN)
/**
 * @brief Maximum amount of chip select lines per bus
 *
 * Allows up to SPI_MAXCS hardware cable select lines per SPI device. The n-th
 * hardware select line can be used with the SPI_HWCS macro.
 */
#  define SPI_MAXCS (4U)
#endif

/**
 * @brief Hardware chip select access macro.
 *
 * The amount of available hardware chip select lines depends on the SPI_MAXCS
 * parameter. If the line is actually available at runtime depends of whether a
 * `--spi` startup parameter with the corresponding SPI device and HWCS-line
 * parameter has been given.
 */
#define SPI_HWCS(x)     (UINT_MAX - SPI_MAXCS + x)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
