/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_stm32f3discovery STM32F3Discovery
 * @ingroup     boards
 * @brief       Board specific files for the STM32F3Discovery board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F3Discovery evaluation board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000UL)

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define the UART used for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_BUFSIZE       (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOE
#define LD3_PIN             (1 << 9)
#define LD4_PIN             (1 << 8)
#define LD5_PIN             (1 << 10)
#define LD6_PIN             (1 << 15)
#define LD7_PIN             (1 << 11)
#define LD8_PIN             (1 << 14)
#define LD9_PIN             (1 << 12)
#define LD10_PIN            (1 << 13)
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
#define LD5_ON              (LED_PORT->BSRRL = LD5_PIN)
#define LD5_OFF             (LED_PORT->BSRRH = LD5_PIN)
#define LD5_TOGGLE          (LED_PORT->ODR ^= LD5_PIN)
#define LD6_ON              (LED_PORT->BSRRL = LD6_PIN)
#define LD6_OFF             (LED_PORT->BSRRH = LD6_PIN)
#define LD6_TOGGLE          (LED_PORT->ODR ^= LD6_PIN)
#define LD7_ON              (LED_PORT->BSRRL = LD7_PIN)
#define LD7_OFF             (LED_PORT->BSRRH = LD7_PIN)
#define LD7_TOGGLE          (LED_PORT->ODR ^= LD7_PIN)
#define LD8_ON              (LED_PORT->BSRRL = LD8_PIN)
#define LD8_OFF             (LED_PORT->BSRRH = LD8_PIN)
#define LD8_TOGGLE          (LED_PORT->ODR ^= LD8_PIN)
#define LD9_ON              (LED_PORT->BSRRL = LD9_PIN)
#define LD9_OFF             (LED_PORT->BSRRH = LD9_PIN)
#define LD9_TOGGLE          (LED_PORT->ODR ^= LD9_PIN)
#define LD10_ON             (LED_PORT->BSRRL = LD10_PIN)
#define LD10_OFF            (LED_PORT->BSRRH = LD10_PIN)
#define LD10_TOGGLE         (LED_PORT->ODR ^= LD10_PIN)
/* for compatability to other boards */
#define LED_GREEN_ON        LD4_ON
#define LED_GREEN_OFF       LD4_OFF
#define LED_GREEN_TOGGLE    LD4_TOGGLE
#define LED_RED_ON          LD5_ON
#define LED_RED_OFF         LD5_OFF
#define LED_RED_TOGGLE      LD5_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
