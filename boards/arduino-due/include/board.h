/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_arduino-due Arduino Due
 * @ingroup     boards
 * @brief       Support for the Arduino Due board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Due board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            PIOB
#define LED_BIT             PIO_PB27
#define LED_PIN             GPIO_PIN(PB, 27)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_ON              (LED_PORT->PIO_SODR = LED_BIT)
#define LED_OFF             (LED_PORT->PIO_CODR = LED_BIT)
#define LED_TOGGLE          (LED_PORT->PIO_ODSR ^= LED_BIT)

/* for compatability to other boards */
#define LED_GREEN_ON        LED_ON
#define LED_GREEN_OFF       LED_OFF
#define LED_GREEN_TOGGLE    LED_TOGGLE
#define LED_RED_ON          /* not available */
#define LED_RED_OFF         /* not available */
#define LED_RED_TOGGLE      /* not available */
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H_ */
/** @} */
