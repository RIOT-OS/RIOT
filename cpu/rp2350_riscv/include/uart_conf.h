/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350_riscv
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

/** UART baud rate in bits per second */
#define BAUDRATE 115200u

/** Integer baud rate divisor */
#define IBRD ((((8u * CPUFREQ) + BAUDRATE) / (2u * BAUDRATE)) / 64u)

/** Fractional baud rate divisor */
#define FBRD ((((8u * CPUFREQ) + BAUDRATE) / (2u * BAUDRATE)) % 64u)

/** UART enable bit in control register */
#define UART_UARTCR_UARTEN_BITS (1u << 0u)

/** UART receive enable bit in control register */
#define UART_UARTCR_RXE_BITS (1u << 9u)

/** UART transmit enable bit in control register */
#define UART_UARTCR_TXE_BITS (1u << 8u)

/** UART receive FIFO full flag bit in flag register */
#define UART_UARTFR_RXFF_BITS (1u << 6u)

/** UART transmit FIFO empty flag bit in flag register */
#define UART_UARTFR_TXFE_BITS (1u << 7u)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

/** @} */
