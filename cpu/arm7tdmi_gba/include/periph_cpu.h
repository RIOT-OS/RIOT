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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Clock configuration
 */
#define CLOCK_CORECLOCK              (16780000U) /* 16.78MHz */

#define TIMER_NUMOF                  (4u)
#define TIMER_CHANNEL_NUMOF          1U
#define CONFIG_ZTIMER_USEC_BASE_FREQ (16400LU)

/**
 * @brief   A low-level timer_set() implementation is provided
 */
#define PERIPH_TIMER_PROVIDES_SET

#ifdef __cplusplus
}
#endif

/** @} */
