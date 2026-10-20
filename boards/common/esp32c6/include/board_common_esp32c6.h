/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32c6
 * @brief       Board definitions that are common for all ESP32-C6 boards.
 *
 * This file contains board configurations that are valid for all ESP32-C6.
 *
 * For detailed information about the configuration of ESP32-C6 boards, see
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
 * @name    ztimer Configuration valid for all ESP32-C6 boards
 * @{
 */
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 20
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         32
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       37
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         16
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       18
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         8
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       9
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 120
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         6
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       6
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         5
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       5
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
