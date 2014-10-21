/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_pca10000
 * @ingroup     boards
 * @brief       Board specific files for the nRF51822 board pca10000.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nRF51822 evaluation board pca10000.
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
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
 * @name Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define ONBOARD_LED         1
#define LED_RED_PIN         (1 << 21)
#define LED_GREEN_PIN       (1 << 22)
#define LED_BLUE_PIN        (1 << 23)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (NRF_GPIO->OUTCLR = LED_RED_PIN)
#define LED_RED_OFF         (NRF_GPIO->OUTSET = LED_RED_PIN)
#define LED_RED_TOGGLE      (NRF_GPIO->OUT ^= LED_RED_PIN)
#define LED_GREEN_ON        (NRF_GPIO->OUTCLR = LED_GREEN_PIN)
#define LED_GREEN_OFF       (NRF_GPIO->OUTSET = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    (NRF_GPIO->OUT ^= LED_GREEN_PIN)
#define LED_BLUE_ON         (NRF_GPIO->OUTCLR = LED_BLUE_PIN)
#define LED_BLUE_OFF        (NRF_GPIO->OUTSET = LED_BLUE_PIN)
#define LED_BLUE_TOGGLE     (NRF_GPIO->OUT ^= LED_BLUE_PIN)


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
