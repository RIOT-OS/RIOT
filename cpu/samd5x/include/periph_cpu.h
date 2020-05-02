/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_samd5x
 * @brief           CPU specific definitions for internal peripheral handling
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <limits.h>

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   DFLL runs at at fixed frequency of 48 MHz
 */
#define SAM0_DFLL_FREQ_HZ       (48000000U)

/**
 * @brief   DPLL must run with at least 96 MHz
 */
#define SAM0_DPLL_FREQ_MIN_HZ   (96000000U)

/**
 * @brief   DPLL frequency must not exceed 200 MHz
 */
#define SAM0_DPLL_FREQ_MAX_HZ   (200000000U)

/**
 * @name    Power mode configuration
 * @{
 */
#define PM_NUM_MODES        (3)
/** @} */

/**
 * @name   SAMD5x GCLK definitions
 * @{
 */
enum {
    SAM0_GCLK_MAIN = 0,                 /**< 120 MHz main clock     */
    SAM0_GCLK_32KHZ,                    /**< 32 kHz clock           */
    SAM0_GCLK_8MHZ,                     /**< 8 MHz clock for xTimer */
    SAM0_GCLK_48MHZ,                    /**< 48 MHz DFLL clock      */
};
/** @} */

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

/**
 * @brief   The MCU has a 12 bit DAC
 */
#define DAC_RES_BITS        (12)

/**
 * @brief   The MCU has two DAC outputs.
 */
#define DAC_NUMOF           (2)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
