/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32c3
 * @brief       Board definitions that are common for all ESP32-C3 boards.
 *
 * This file contains board configurations that are valid for all ESP32-C3.
 *
 * For detailed information about the configuration of ESP32-C3 boards, see
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
 * @name    ztimer Configuration valid for all ESP32-C3 boards
 * @{
 */
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 2
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         331
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       361
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 5
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         132
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       144
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 10
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         66
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       71
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 20
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         33
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       36
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         16
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       18
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         8
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       8
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         5
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       4
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
