/*
 * SPDX-FileCopyrightText: 2021 Benjamin Valentin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_zigduino
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Zigduino board.
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
#define STDIO_UART_BAUDRATE (57600U)      /**< Sets Baudrate for e.g. Shell */
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

/**
 * @name    Grove PIR Motion sensor
 * @{
 */
#define PIR_MOTION_PIN      GPIO_PIN(PORT_D, 4)
#define PIR_MOTION_MODE     GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
