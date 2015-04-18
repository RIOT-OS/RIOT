/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_openmote OpenMote
 * @ingroup     boards
 * @brief       Support for the OpenMote board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenMote board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (32000000UL)

/**
 * @name Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIO_C
#define LED_RED_PIN         (4)
#define LED_GREEN_PIN       (7)
#define LED_YELLOW_PIN      (6)
#define LED_ORANGE_PIN      (5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_PORT->DATA |= (1 << LED_RED_PIN))
#define LED_RED_OFF         (LED_PORT->DATA &= ~(1 << LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_PORT->DATA ^= (1 << LED_RED_PIN))

#define LED_GREEN_ON        (LED_PORT->DATA |= (1 << LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_PORT->DATA &= ~(1 << LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_PORT->DATA ^= (1 << LED_GREEN_PIN))

#define LED_YELLOW_ON       (LED_PORT->DATA |= (1 << LED_YELLOW_PIN))
#define LED_YELLOW_OFF      (LED_PORT->DATA &= ~(1 << LED_YELLOW_PIN))
#define LED_YELLOW_TOGGLE   (LED_PORT->DATA ^= (1 << LED_YELLOW_PIN))

#define LED_ORANGE_ON       (LED_PORT->DATA |= (1 << LED_ORANGE_PIN))
#define LED_ORANGE_OFF      (LED_PORT->DATA &= ~(1 << LED_ORANGE_PIN))
#define LED_ORANGE_TOGGLE   (LED_PORT->DATA ^= (1 << LED_ORANGE_PIN))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /** __BOARD_H */
/** @} */
