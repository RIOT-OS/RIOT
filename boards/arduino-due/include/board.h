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
#define LED0_PIN            GPIO_PIN(PB, 27)

#define LED0_ON             (PIOB->PIO_SODR  = PIO_PB27)
#define LED0_OFF            (PIOB->PIO_CODR  = PIO_PB27)
#define LED0_TOGGLE         (PIOB->PIO_ODSR ^= PIO_PB27)
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
