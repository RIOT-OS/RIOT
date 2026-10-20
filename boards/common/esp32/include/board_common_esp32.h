/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32
 * @brief       Board definitions that are common for all ESP32 boards.
 *
 * This file contains board configurations that are valid for all ESP32.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

/**
 * @brief   External clock crystal frequency (MHz)
 *
 * Most boards use a 40MHz crystal, including all those based on Espressif's
 * WROOM-32 and WROVER-32 modules. Those that don't, like the SparkFun ESP32
 * Thing (26MHz), must define ESP32_XTAL_FREQ appropriately in their board
 * configuration.
 *
 * An obvious side effect of a mismatch is that the UART won't sync and the
 * development terminal will show garbage instead of log output.
 */
#ifndef ESP32_XTAL_FREQ
#define ESP32_XTAL_FREQ             (40)
#endif

#if !DOXYGEN

/**
 * @name    ztimer Configuration valid for all ESP32 boards
 * @{
 */
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 2
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         465
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       555
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 5
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         185
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       231
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 10
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         92
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       116
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 20
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         45
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       59
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 40
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         22
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       30
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         11
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       15
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 160
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         6
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       7
#elif CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ == 240
#  define CONFIG_ZTIMER_USEC_ADJUST_SET         4
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP       5
#else
#  error "Invalid CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ value"
#endif
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
