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
#define TIMER_NUMOF         (4U) /**< Number of timers */
#define TIMER_CHANNEL_NUMOF (1U) /**< Number of channels */
#define TIMER_0_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_1_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_2_MAX_VALUE   (0xffff) /**< max value */
#define TIMER_3_MAX_VALUE   (0xffff) /**< max value */
#define PERIPH_TIMER_PROVIDES_SET /**< A low-level timer_set() implementation is provided */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
