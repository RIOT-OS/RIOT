/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_common_esp32
 * @brief       Common declarations of ESP32 periphery for all ESP32 boards
 *
 * This file contains peripheral configurations that are valid for all ESP32.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_peripherals "Common Peripherals".
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
#define UART0_TXD   (GPIO1)  /**< TxD of UART_DEV(0) used on all ESP32 boards */
#endif

#ifndef UART0_RXD
#define UART0_RXD   (GPIO3)  /**< RxD of UART_DEV(0) used on all ESP32 boards */
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
