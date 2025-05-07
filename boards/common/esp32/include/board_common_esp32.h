/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ <= 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET   (880/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP ((CONFIG_ZTIMER_USEC_ADJUST_SET >> 2) * 5)
#else
#  define CONFIG_ZTIMER_USEC_ADJUST_SET   (960/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP (CONFIG_ZTIMER_USEC_ADJUST_SET + 1)
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
