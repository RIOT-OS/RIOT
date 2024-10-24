/*
 * Copyright (C) 2016-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf52dk
 * @{
 *
 * @file
 * @brief       Board specific configuration for the nRF52 DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 17)
#define LED1_PIN            GPIO_PIN(0, 18)
#define LED2_PIN            GPIO_PIN(0, 19)
#define LED3_PIN            GPIO_PIN(0, 20)

#define LED_PORT            (NRF_P0)
#define LED0_MASK           (1 << 17)
#define LED1_MASK           (1 << 18)
#define LED2_MASK           (1 << 19)
#define LED3_MASK           (1 << 20)
#define LED_MASK            (LED0_MASK | LED1_MASK | LED2_MASK | LED3_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)

#define LED2_ON             (LED_PORT->OUTCLR = LED2_MASK)
#define LED2_OFF            (LED_PORT->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (LED_PORT->OUT   ^= LED2_MASK)

#define LED3_ON             (LED_PORT->OUTCLR = LED3_MASK)
#define LED3_OFF            (LED_PORT->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (LED_PORT->OUT   ^= LED3_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 13)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(0, 14)
#define BTN1_MODE           GPIO_IN_PU
#define BTN2_PIN            GPIO_PIN(0, 15)
#define BTN2_MODE           GPIO_IN_PU
#define BTN3_PIN            GPIO_PIN(0, 16)
#define BTN3_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
