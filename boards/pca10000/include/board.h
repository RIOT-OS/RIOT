/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_stm32f0discovery STM32F0Discovery
 * @ingroup     boards
 * @brief       Board specific files for the STM32F0Discovery board.
 * @{
 *
 * @file        board.h
 * @brief       Board specific definitions for the STM32F0Discovery evaluation board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"


/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (16000000UL)

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name LED pin definitions
 * @{
 */
#define LED_RED_PIN         (1 << 21)
#define LED_GREEN_PIN       (1 << 22)
#define LED_BLUE_PIN        (1 << 23)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_OFF          NRF_GPIO->OUTCLR = LED_RED_PIN
#define LED_RED_ON         NRF_GPIO->OUTSET = LED_RED_PIN
#define LED_RED_TOGGLE      NRF_GPIO->OUT ^= LED_RED_PIN
#define LED_GREEN_OFF        NRF_GPIO->OUTCLR = LED_GREEN_PIN
#define LED_GREEN_ON       NRF_GPIO->OUTSET = LED_GREEN_PIN
#define LED_GREEN_TOGGLE    NRF_GPIO->OUT ^= LED_GREEN_PIN
#define LED_BLUE_OFF        NRF_GPIO->OUTCLR = LED_BLUE_PIN
#define LED_BLUE_ON        NRF_GPIO->OUTSET = LED_BLUE_PIN
#define LED_BLUE_TOGGLE     NRF_GPIO->OUT ^= LED_BLUE_PIN


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

void delay(uint32_t microseconds);


#endif /** __BOARD_H */
/** @} */
