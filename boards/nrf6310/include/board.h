/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_nrf6310 NRF6310 (Nordic NRF Hardware Development Kit)
 * @ingroup     boards
 * @brief       Board specific files for the nRF51 boards nrf6310 or MOMMOSOFT BLE DEVKIT.N
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nRF51 evaluation board nrf6310
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions
 * @{
 */
#define LED_RED_PIN         (GPIO_PIN(0, 8))
#define LED_GREEN_PIN       (GPIO_PIN(0, 9))
#define LED_BLUE_PIN        (GPIO_PIN(0, 10))
#define LED_RED_MASK        (1 << 8)
#define LED_GREEN_MASK      (1 << 9)
#define LED_BLUE_MASK       (1 << 10)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (NRF_GPIO->OUTCLR = LED_RED_MASK)
#define LED_RED_OFF         (NRF_GPIO->OUTSET = LED_RED_MASK)
#define LED_RED_TOGGLE      (NRF_GPIO->OUT   ^= LED_RED_MASK)
#define LED_GREEN_ON        (NRF_GPIO->OUTCLR = LED_GREEN_MASK)
#define LED_GREEN_OFF       (NRF_GPIO->OUTSET = LED_GREEN_MASK)
#define LED_GREEN_TOGGLE    (NRF_GPIO->OUT   ^= LED_GREEN_MASK)
#define LED_BLUE_ON         (NRF_GPIO->OUTCLR = LED_BLUE_MASK)
#define LED_BLUE_OFF        (NRF_GPIO->OUTSET = LED_BLUE_MASK)
#define LED_BLUE_TOGGLE     (NRF_GPIO->OUT   ^= LED_BLUE_MASK)
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
