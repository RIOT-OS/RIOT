/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Frank Holtz <frank-riot2015@holtznet.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nrf52-minidev nRF52 minidev
 * @ingroup     boards
 * @{
 *
 * @file
 * @brief       Board specific configuaration for the nRF52 minidev
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 30)
#define LED1_PIN            GPIO_PIN(0, 31)

#define LED0_MASK           (1 << 17)
#define LED1_MASK           (1 << 18)

#define LED0_ON             (NRF_P0->OUTCLR = LED0_MASK)
#define LED0_OFF            (NRF_P0->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (NRF_P0->OUT   ^= LED0_MASK)

#define LED1_ON             (NRF_P0->OUTCLR = LED1_MASK)
#define LED1_OFF            (NRF_P0->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (NRF_P0->OUT   ^= LED1_MASK)
/** @} */

/**
 * @brief   Button pin configuration
 * @{
 */
#define BUTTON1_PIN         (GPIO_PIN(0, 13))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** BOARD_H */
/** @} */
