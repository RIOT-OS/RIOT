/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32s2
 * @brief       Board definitions that are common for all ESP32-S2 boards.
 *
 * This file contains board configurations that are valid for all ESP32-S2.
 *
 * For detailed information about the configuration of ESP32-S2 boards, see
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
 * @name    ztimer Configuration valid for all ESP32-S2 boards
 * @{
 */
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 2
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         456
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       537
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 5
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         181
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       221
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 10
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         90
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       111
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 20
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         45
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       55
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         22
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       28
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         11
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       14
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         6
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       7
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 240
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
