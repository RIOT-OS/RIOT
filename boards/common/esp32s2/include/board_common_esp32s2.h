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
#if CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ <= 80
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (880/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   ((CONFIG_ZTIMER_USEC_ADJUST_SET >> 2) * 5)
#else
#  define CONFIG_ZTIMER_USEC_ADJUST_SET     (960/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#  define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (CONFIG_ZTIMER_USEC_ADJUST_SET + 1)
#endif
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
