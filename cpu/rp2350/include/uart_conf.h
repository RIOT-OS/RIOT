/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           UART configuration for the RP2350
 *
 * @author          Tom Hert <git@annsann.eu>
 */

#include "RP2350.h"
#include "macros/units.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief UART baud rate in bits per second
 */
#define BAUDRATE 115200

/**
 * @brief Integer baud rate divisor
 * Calculated based on CPU frequency and desired baud rate
 * Formula: ((8 * CPUFREQ + BAUDRATE) / (2 * BAUDRATE)) / 64
 */
#define IBRD ((8 * CPUFREQ + BAUDRATE) / (2 * BAUDRATE)) / 64

/**
 * @brief Fractional baud rate divisor
 * Calculated based on CPU frequency and desired baud rate
 * Formula: ((8 * CPUFREQ + BAUDRATE) / (2 * BAUDRATE)) % 64
 */
#define FBRD ((8 * CPUFREQ + BAUDRATE) / (2 * BAUDRATE)) % 64

/**
 * @brief UART enable bit in control register
 * Bit 0 of UARTCR register
 */
#define UART_UARTCR_UARTEN_BITS 1 << 0

/**
 * @brief UART receive enable bit in control register
 * Bit 9 of UARTCR register
 */
#define UART_UARTCR_RXE_BITS 1 << 9

/**
 * @brief UART transmit enable bit in control register
 * Bit 8 of UARTCR register
 */
#define UART_UARTCR_TXE_BITS 1 << 8

/**
 * @brief UART receive FIFO full flag bit in flag register
 * Bit 6 of UARTFR register
 */
#define UART_UARTFR_RXFF_BITS 1 << 6

/**
 * @brief UART transmit FIFO empty flag bit in flag register
 * Bit 7 of UARTFR register
 */
#define UART_UARTFR_TXFE_BITS 1 << 7

/** @} */
