/*
 * Copyright (C) 2022 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @ingroup     config
 * @brief       Compile-time configuration macros for ESP32-S3 SoCs
 * @{
 *
 * @file
 * @brief       ESP32-S3 specific compile-time configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ESP_ISR_STACKSIZE
/** Stack size used in interrupt context */
#define ESP_ISR_STACKSIZE               (THREAD_STACKSIZE_DEFAULT)
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
