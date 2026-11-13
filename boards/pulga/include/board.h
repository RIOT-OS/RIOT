/*
 * Copyright (C) 2022 Universidade de São Paulo
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_pulga
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Pulga board
 *
 * @author      Geovane Fedrecheski <geonnave@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"
#include "mtd.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(1, 13)
#define LED1_PIN            GPIO_PIN(1, 14)

#define LED_PORT            (NRF_P1)
#define LED0_MASK           (1 << 13)
#define LED1_MASK           (1 << 14)
#define LED_MASK            (LED0_MASK | LED1_MASK)

#define LED0_OFF            (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_ON             (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_OFF            (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_ON             (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(1, 12)
#define BTN0_MODE           GPIO_IN_PU
#define BTN1_PIN            GPIO_PIN(1, 11)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
