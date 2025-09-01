/*
 * SPDX-FileCopyrightText: 2019 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_microduino-corerf
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Microduino CoreRF board.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    baudrate for STDIO terminal
 *
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (115200U)      /**< Sets Baudrate for e.g. Shell */
#endif
/** @} */

/**
 * @name    xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (250000UL)
#define XTIMER_BACKOFF      (40)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
