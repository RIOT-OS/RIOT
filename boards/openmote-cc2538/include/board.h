/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_openmote-cc2538 OpenMote-cc2538
 * @ingroup     boards
 * @brief       Support for the OpenMote-cc2538 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenMote-cc2538 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIO_C
#define LED_RED_PIN         (4)
#define LED_GREEN_PIN       (7)
#define LED_YELLOW_PIN      (6)
#define LED_ORANGE_PIN      (5)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_PORT->DATA |= (1 << LED_RED_PIN))
#define LED_RED_OFF         (LED_PORT->DATA &= ~(1 << LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_PORT->DATA ^= (1 << LED_RED_PIN))

#define LED_GREEN_ON        (LED_PORT->DATA |= (1 << LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_PORT->DATA &= ~(1 << LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_PORT->DATA ^= (1 << LED_GREEN_PIN))

#define LED_YELLOW_ON       (LED_PORT->DATA |= (1 << LED_YELLOW_PIN))
#define LED_YELLOW_OFF      (LED_PORT->DATA &= ~(1 << LED_YELLOW_PIN))
#define LED_YELLOW_TOGGLE   (LED_PORT->DATA ^= (1 << LED_YELLOW_PIN))

#define LED_ORANGE_ON       (LED_PORT->DATA |= (1 << LED_ORANGE_PIN))
#define LED_ORANGE_OFF      (LED_PORT->DATA &= ~(1 << LED_ORANGE_PIN))
#define LED_ORANGE_TOGGLE   (LED_PORT->DATA ^= (1 << LED_ORANGE_PIN))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
/** @} */
