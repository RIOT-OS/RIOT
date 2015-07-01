/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nrf52-previewdk nrf52-previewdk
 * @ingroup     boards
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nRF52832 board nrf52-previewdk
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (64000000UL)

/**
 * @brief Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define the boards stdio
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
#define ONBOARD_LED         1
#define LED_ONE_PIN         (1 << 17)
#define LED_TWO_PIN         (1 << 18)
#define LED_THREE_PIN       (1 << 19)
#define LED_FOUR_PIN        (1 << 20)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_ONE_ON          (NRF_P0->OUTCLR = LED_ONE_PIN)
#define LED_ONE_OFF         (NRF_P0->OUTSET = LED_ONE_PIN)
#define LED_ONE_TOGGLE      (NRF_P0->OUT ^= LED_ONE_PIN)
#define LED_TWO_ON          (NRF_P0->OUTCLR = LED_TWO_PIN)
#define LED_TWO_OFF         (NRF_P0->OUTSET = LED_TWO_PIN)
#define LED_TWO_TOGGLE      (NRF_P0->OUT ^= LED_TWO_PIN)
#define LED_THREE_ON        (NRF_P0->OUTCLR = LED_THREE_PIN)
#define LED_THREE_OFF       (NRF_P0->OUTSET = LED_THREE_PIN)
#define LED_THREE_TOGGLE    (NRF_P0->OUT ^= LED_THREE_PIN)
#define LED_FOUR_ON         (NRF_P0->OUTCLR = LED_FOUR_PIN)
#define LED_FOUR_OFF        (NRF_P0->OUTSET = LED_FOUR_PIN)
#define LED_FOUR_TOGGLE     (NRF_P0->OUT ^= LED_FOUR_PIN)
/** @} */


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
