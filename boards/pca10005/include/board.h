/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_pca10005
 * @ingroup     boards
 * @brief       Board specific files for the nRF51822 board pca10005
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nRF51822 evaluation board pca10005
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"

/**
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               (16000000UL)

/**
 * @name Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
/** @} */

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_PIN         1

#define LED_RED_ON          (NRF_GPIO->OUTSET = (1 << LED_RED_PIN))
#define LED_RED_OFF         (NRF_GPIO->OUTCLR = (1 << LED_RED_PIN))
#define LED_RED_TOGGLE      (NRF_GPIO->OUT ^= (1 << LED_RED_PIN))
#define LED_GREEN_ON        /* not available */
#define LED_GREEN_OFF       /* not available */
#define LED_GREEN_TOGGLE    /* not available */
#define LED_BLUE_ON         /* not available */
#define LED_BLUE_OFF        /* not available */
#define LED_BLUE_TOGGLE     /* not available */
/* @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
