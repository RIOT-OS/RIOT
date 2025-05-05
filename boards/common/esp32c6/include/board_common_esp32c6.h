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
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         134
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       139
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         33
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       34
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         18
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       18
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 120
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         12
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       13
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         11
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       10
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
