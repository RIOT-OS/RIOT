/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_f4vi1  F4VI1
 * @ingroup     boards
 * @brief       Board specific files for the F4VI1 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the f4vi1 board
 *
 * @author      Stefan Pfeiffer <pfeiffer@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Define UART device and baudrate for stdio
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
#define LED_PORT            GPIOA
#define LD4_PIN             (1 << 1)
#define LD5_PIN             (1 << 3)
#define LD6_PIN             (1 << 2)

/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LD4_ON              (LED_PORT->BSRRH = LD4_PIN)
#define LD4_OFF             (LED_PORT->BSRRL = LD4_PIN)
#define LD4_TOGGLE          (LED_PORT->ODR ^= LD4_PIN)
#define LD5_ON              (LED_PORT->BSRRH = LD5_PIN)
#define LD5_OFF             (LED_PORT->BSRRL = LD5_PIN)
#define LD5_TOGGLE          (LED_PORT->ODR ^= LD5_PIN)
#define LD6_ON              (LED_PORT->BSRRH = LD6_PIN)
#define LD6_OFF             (LED_PORT->BSRRL = LD6_PIN)
#define LD6_TOGGLE          (LED_PORT->ODR ^= LD6_PIN)

/* for compatability to other boards */
#define LED_GREEN_ON        LD6_ON
#define LED_GREEN_OFF       LD6_OFF
#define LED_GREEN_TOGGLE    LD6_TOGGLE
#define LED_RED_ON          LD5_ON
#define LED_RED_OFF         LD5_OFF
#define LED_RED_TOGGLE      LD5_TOGGLE
#define LED_ORANGE_ON       LD4_ON
#define LED_ORANGE_OFF      LD4_OFF
#define LED_ORANGE_TOGGLE   LD4_TOGGLE
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H_ */
/** @} */
