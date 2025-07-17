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
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         14
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       13
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 64
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         19
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       18
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 48
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         34
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       32
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 32
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         32
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       33
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 16
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         129
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       135
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif

/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
