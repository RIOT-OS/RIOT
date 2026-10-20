/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32s3
 * @brief       Board definitions that are common for all ESP32-S3 boards.
 *
 * This file contains board configurations that are valid for all ESP32-S3.
 *
 * For detailed information about the configuration of ESP32-S3 boards, see
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
 * @name    ztimer Configuration valid for all ESP32-S3 boards
 * @{
 */
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 2
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         448
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       538
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 5
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         178
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       198
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 10
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         89
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       98
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 20
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         44
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       49
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         22
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       25
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         10
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       13
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         6
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       6
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 240
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         4
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
