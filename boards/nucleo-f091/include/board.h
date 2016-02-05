/*
 * Copyright (C) 2015  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f091 Nucleo-F091
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f091 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f091 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED_GREEN_PORT      (GPIOA)
#define LED_GREEN_PIN       (5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON
#define LED_RED_OFF
#define LED_RED_TOGGLE

#define LED_GREEN_ON        (LED_GREEN_PORT->BSRRL = (1 << LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_GREEN_PORT->BSRRH = (1 << LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_GREEN_PORT->ODR ^= (1 << LED_GREEN_PIN))

#define LED_ORANGE_ON
#define LED_ORANGE_OFF
#define LED_ORANGE_TOGGLE
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
/** @} */
