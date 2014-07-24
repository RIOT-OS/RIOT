/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_stm32f0discovery STM32F0Discovery
 * @ingroup     boards
 * @brief       Support for the STM32F0Discovery board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F0Discovery evaluation board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"


/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (48000000UL)

/**
 * @name Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_BUFSIZE       (64U)

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOC
#define LD3_PIN             (1 << 9)
#define LD4_PIN             (1 << 8)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LD3_ON              (LED_PORT->BSRRL = LD3_PIN)
#define LD3_OFF             (LED_PORT->BSRRH = LD3_PIN)
#define LD3_TOGGLE          (LED_PORT->ODR ^= LD3_PIN)
#define LD4_ON              (LED_PORT->BSRRL = LD4_PIN)
#define LD4_OFF             (LED_PORT->BSRRH = LD4_PIN)
#define LD4_TOGGLE          (LED_PORT->ODR ^= LD4_PIN)

/* for compatibility to other boards */
#define LED_GREEN_ON        LD4_ON
#define LED_GREEN_OFF       LD4_OFF
#define LED_GREEN_TOGGLE    LD4_TOGGLE
#define LED_RED_ON          LD3_ON
#define LED_RED_OFF         LD3_OFF
#define LED_RED_TOGGLE      LD3_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
