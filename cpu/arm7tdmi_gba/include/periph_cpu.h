/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_arm7tdmi_gba
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author
 */

#include <stdint.h>

#include "periph_gba.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Clock configuration
 */
#define CLOCK_CORECLOCK     (16780000U) /* 16.78MHz */

/**
 * @name Timer API defines
 * @{
 */
#define TIMER_NUMOF         (4U)     /**< Number of timers */
#define TIMER_CHANNEL_NUMOF (1U)     /**< Number of channels */
#define TIMER_0_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_1_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_2_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_3_MAX_VALUE   (0xffff) /**< max value */
#define PERIPH_TIMER_PROVIDES_SET    /**< A low-level timer_set() implementation is provided */
/** @} */

/**
 * @name    Ztimer configuration
 *
 * The timer runs at 262.2 kHz, 3.8 µs resolution, overflow IRQ every 250 ms (best case).
 *
 * A more precise option would be 16.78MHz, 60 ns resolution, overflow IRQ every 4 ms (best case)
 * (which is too frequent for most applications).
 * A less precise option would be 65.5 kHz, 15.2 µs resolution, overflow IRQ every 1 s
 * (which is too slow compared to most other systems, might not match expectations).
 *
 * @{
 */
#define CONFIG_ZTIMER_USEC_DEV          (TIMER_DEV(0))       /**< Timer peripheral for ztimer */
#define CONFIG_ZTIMER_USEC_BASE_FREQ    (GBA_TIMER_FRQ_262K) /**< Running at 262.2 kHz */
#define CONFIG_ZTIMER_USEC_WIDTH        (16)                 /**< Running on a 16-bit timer */
#define CONFIG_ZTIMER_USEC_ADJUST_SET   (308)                /**< Overhead for ztimer_set */
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP (312)                /**< Overhead for ztimer_sleep */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
