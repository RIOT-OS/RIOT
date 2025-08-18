/*
 * SPDX-FileCopyrightText: 2022 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32s2
 * @brief       Common peripheral configurations for ESP32-S2 boards
 *
 * This file contains the peripheral configurations that are valid for all
 * ESP32-S2 boards.
 *
 * For detailed information about the peripheral configuration for ESP32-S2
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
#define UART0_TXD   (GPIO43)  /**< TxD of UART_DEV(0) used on all ESP32-S2 boards */
#endif

#ifndef UART0_RXD
#define UART0_RXD   (GPIO44)  /**< RxD of UART_DEV(0) used on all ESP32-S2 boards */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
