/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @ingroup     config
 * @brief       Compile-time configuration macros for ESP32-H2 SoCs
 * @{
 *
 * @file
 * @brief       ESP32-H2 specific compile-time configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_ISR_STACKSIZE
/** Stack size used in interrupt context */
#  define ESP_ISR_STACKSIZE             (THREAD_STACKSIZE_DEFAULT)
#endif /* ESP_ISR_STACKSIZE */

/** Number of DRAM sections that can be used as heap. */
#define NUM_HEAPS (1)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
