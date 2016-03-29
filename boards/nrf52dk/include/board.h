/*
 * Copyright (C) 2016 Feie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nrf52dk nRF52 DK
 * @ingroup     boards
 * @{
 *
 * @file
 * @brief       Board specific configuaration for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define LED0_PIN            GPIO_PIN(0, 17)
#define LED1_PIN            GPIO_PIN(0, 18)
#define LED2_PIN            GPIO_PIN(0, 19)
#define LED3_PIN            GPIO_PIN(0, 20)

#define LED0_MASK           (1 << 17)
#define LED1_MASK           (1 << 18)
#define LED2_MASK           (1 << 19)
#define LED3_MASK           (1 << 20)

#define LED0_ON             (NRF_P0->OUTCLR = LED0_MASK)
#define LED0_OFF            (NRF_P0->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (NRF_P0->OUT   ^= LED0_MASK)

#define LED1_ON             (NRF_P0->OUTCLR = LED1_MASK)
#define LED1_OFF            (NRF_P0->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (NRF_P0->OUT   ^= LED1_MASK)

#define LED2_ON             (NRF_P0->OUTCLR = LED2_MASK)
#define LED2_OFF            (NRF_P0->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (NRF_P0->OUT   ^= LED2_MASK)

#define LED3_ON             (NRF_P0->OUTCLR = LED3_MASK)
#define LED3_OFF            (NRF_P0->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (NRF_P0->OUT   ^= LED3_MASK)
/** @} */

/**
 * @brief   Button pin configuration
 * @{
 */
#define BUTTON1_PIN         (GPIO_PIN(0, 13))
#define BUTTON2_PIN         (GPIO_PIN(0, 14))
#define BUTTON3_PIN         (GPIO_PIN(0, 15))
#define BUTTON4_PIN         (GPIO_PIN(0, 16))
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
