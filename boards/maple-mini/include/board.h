/*
 * Copyright (C) 2016 Frits Kuipers
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_maple-mini
 * @{
 *
 * @file
 * @brief       Board specific definitions for the maple-mini board
 *
 * @author      Frits Kuipers <frits.kuipers@gmail.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

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
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED0_PIN_NUM        1
#define LED0_PORT_NUM       PORT_B
/** @} */

/**
 * @name    User button
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_B, 8)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief Use the USART1 for STDIO on this board
 */
#define STDIO_UART_DEV      UART_DEV(1)

#ifdef __cplusplus
}
#endif

#include "stm32_leds.h"

#endif /* BOARD_H */
/** @} */
