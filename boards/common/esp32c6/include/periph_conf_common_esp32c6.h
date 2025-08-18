/*
 * SPDX-FileCopyrightText: 2025 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32c6
 * @brief       Common peripheral configurations for ESP32-C6 boards
 *
 * This file contains the peripheral configurations that are valid for all
 * ESP32-C6 boards.
 *
 * For detailed information about the peripheral configuration for ESP32-C6
 * boards, see section \ref esp32_peripherals "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   UART configuration
 * @{
 */
#ifndef UART0_TXD
#  define UART0_TXD (GPIO16)  /**< TxD of UART_DEV(0) used on all ESP32-C6 boards */
#endif

#ifndef UART0_RXD
#  define UART0_RXD (GPIO17)  /**< RxD of UART_DEV(0) used on all ESP32-C6 boards */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
