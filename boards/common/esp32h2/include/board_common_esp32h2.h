/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32h2
 * @brief       Board definitions that are common for all ESP32-H2 boards.
 *
 * This file contains board configurations that are valid for all ESP32-H2.
 *
 * For detailed information about the configuration of ESP32-H2 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#if !DOXYGEN

/**
 * @name    ztimer Configuration valid for all ESP32-H2 boards
 * @{
 */

#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 96
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         9
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       9
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 64
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         13
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       12
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 48
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         17
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       16
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 32
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         21
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       22
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 16
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         43
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       45
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif

/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
