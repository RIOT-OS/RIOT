/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_iot-lab_M3 iot-lab_M3
 * @ingroup     boards
 * @brief       Board specific files for the iot-lab_M3 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the iot-lab_M3 board.
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000UL)

/**
 * @name Define the UART to be used as stdio and its baudrate
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name LED pin definitions
 * @{
 */
#define LED_RED_PORT        (GPIOD)
#define LED_RED_PIN         (2)
#define LED_GREEN_PORT      (GPIOB)
#define LED_GREEN_PIN       (5)
#define LED_ORANGE_PORT     (GPIOC)
#define LED_ORANGE_PIN      (10)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_RED_PORT->ODR &= ~(1<<LED_RED_PIN))
#define LED_RED_OFF         (LED_RED_PORT->ODR |= (1<<LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_RED_PORT->ODR ^= (1<<LED_RED_PIN))

#define LED_GREEN_ON        (LED_GREEN_PORT->ODR &= ~(1<<LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_GREEN_PORT->ODR |= (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1<<LED_GREEN_PIN))

#define LED_ORANGE_ON       (LED_ORANGE_PORT->ODR &= ~(1<<LED_ORANGE_PIN))
#define LED_ORANGE_OFF      (LED_ORANGE_PORT->ODR |= (1<<LED_ORANGE_PIN))
#define LED_ORANGE_TOGGLE   (LED_ORANGE_PORT->ODR ^= (1<<LED_ORANGE_PIN))
/** @} */

/**
 * Define the type for the radio packet length for the transceiver
 */
typedef uint8_t radio_packet_length_t;

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /* BOARD_H_ */
/** @} */
