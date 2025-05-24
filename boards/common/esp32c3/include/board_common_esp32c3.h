/*
 * Copyright (C) 2025 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#define CONFIG_ZTIMER_USEC_ADJUST_SET   (640/CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ)
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP (CONFIG_ZTIMER_USEC_ADJUST_SET)
/** @} */

#endif /* !DOXYGEN */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
