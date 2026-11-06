/*
 * SPDX-FileCopyrightText: 2026 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific UART functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#include <stdint.h>

#include "periph/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the wake-up threshold of a UART for light sleep
 *
 * The UART wakes up the system from light sleep if the number of positive
 * edges on its RxD signal is equal to or greater than the given threshold.
 * This function replaces the ESP-IDF function `uart_set_wakeup_threshold`.
 *
 * @param[in]   uart        UART device
 * @param[in]   threshold   number of positive edges on the RxD signal
 *                          required for wake-up
 */
void esp_uart_set_wakeup_threshold(uart_t uart, uint32_t threshold);

#ifdef __cplusplus
}
#endif

/** @} */
