/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sipeed_longan_nano
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Sipeed Longan Nano board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef BOARD_H
#define BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED (on-board) configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_C, 13)
#define LED0_MASK           (1 << 13)
#define LED0_ON             (GPIOC->BC = LED0_MASK)
#define LED0_OFF            (GPIOC->BOP = LED0_MASK)
#define LED0_TOGGLE         (GPIOC->OCTL ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(PORT_A, 1)
#define LED1_MASK           (1 << 1)
#define LED1_ON             (GPIOA->BC = LED1_MASK)
#define LED1_OFF            (GPIOA->BOP = LED1_MASK)
#define LED1_TOGGLE         (GPIOA->OCTL ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(PORT_A, 2)
#define LED2_MASK           (1 << 2)
#define LED2_ON             (GPIOA->BC = LED2_MASK)
#define LED2_OFF            (GPIOA->BOP = LED2_MASK)
#define LED2_TOGGLE         (GPIOA->OCTL ^= LED2_MASK)

#define LED_RED_PIN         LED0_PIN    /**< LED0 is red */
#define LED_GREEN_PIN       LED1_PIN    /**< LED1 is green */
#define LED_BLUE_PIN        LED2_PIN    /**< LED2 is blue */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
