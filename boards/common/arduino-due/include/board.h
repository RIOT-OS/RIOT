/*
 * Copyright (C) 2014,2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_arduino_due Arduino Due Common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for Arduino Due based boards.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Arduino Due based boards
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

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
#define LED0_TOGGLE         ((PIOB->PIO_ODSR & PIO_PB27) ? LED0_OFF : LED0_ON)
/** @} */

/**
 * @name    Configuration parameters for the W5100 driver
 * @{
 */
#ifndef W5100_PARAM_CS
#define W5100_PARAM_CS          (ARDUINO_PIN_10)
#endif
#ifndef W5100_PARAM_EVT
#define W5100_PARAM_EVT         (ARDUINO_PIN_2)
#endif
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
