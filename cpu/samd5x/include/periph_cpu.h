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
 * @brief   Mapping of pins to EXTI lines, -1 means not EXTI possible
 */
static const int8_t exti_config[4][32] = {
#ifdef CPU_MODEL_SAMD51J20A
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, 11, -1, -1, 14, 15 },
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, -1, -1, 14, 15 },
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },
#elif CPU_MODEL_SAME54P20A
    { 0,  1,  2,  3,  4,  5,  6,  7, -1,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, 11, -1, -1, 14, 15 },
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 12, 13, 14, 15, 14, 15 },
    { 0,  1,  2,  3,  4,  5,  6,  9, -1, -1, 10, 11, 12, 13, 14, 15,
      0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, -1, 14, 15 },
    { 0,  1, -1, -1, -1, -1, -1, -1,  3,  4,  5,  6,  7, -1, -1, -1,
     -1, -1, -1, -1, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 }
#else
    #error Please define a proper CPU_MODEL.
#endif
};

/**
 * @brief   Override SPI hardware chip select macro
 *
 * As of now, we do not support HW CS, so we always set it to a fixed value
 */
#define SPI_HWCS(x)     (UINT_MAX - 1)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
