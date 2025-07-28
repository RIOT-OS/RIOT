/*
 * SPDX-FileCopyrightText: 2017 INRIA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_opencm904
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenCM9.04 board
 *
 * @author      Loïc Dauphin <loic.dauphin@inria.fr>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (19)
/** @} */

/**
 * @name Macros for controlling the on-board LED.
 * @{
 */
#define LED0_PIN_NUM        9
#define LED0_PORT           GPIO_PORT_B /**< GPIO port of LED 0 */
#define LED0_PORT_NUM       PORT_B
/** @} */

/**
 * @name User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_C, 15)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief Pin used to switch RX and TX mode for the Dynamixel TTL bus
 *
 * set   = TX mode
 * clear = RX mode
 *
 */
#define DXL_DIR_PIN         GPIO_PIN(PORT_B, 5)

/**
 * @brief Use the USART2 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(0)

/**
 * @name Override with ROBOTIS Bluetooth antenna baudrate for STDIO
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (921600UL)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

/** @} */
