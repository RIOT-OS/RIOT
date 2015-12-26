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
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               (16000000UL)

/**
 * @name Define the boards stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
#define ONBOARD_LED         1
#define LED_RED_PIN         (1 << 8)
#define LED_GREEN_PIN       (1 << 9)
#define LED_BLUE_PIN        (1 << 10)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (NRF_GPIO->OUTCLR = LED_RED_PIN)
#define LED_RED_OFF         (NRF_GPIO->OUTSET = LED_RED_PIN)
#define LED_RED_TOGGLE      (NRF_GPIO->OUT ^= LED_RED_PIN)
#define LED_GREEN_ON        (NRF_GPIO->OUTCLR = LED_GREEN_PIN)
#define LED_GREEN_OFF       (NRF_GPIO->OUTSET = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    (NRF_GPIO->OUT ^= LED_GREEN_PIN)
#define LED_BLUE_ON         (NRF_GPIO->OUTCLR = LED_BLUE_PIN)
#define LED_BLUE_OFF        (NRF_GPIO->OUTSET = LED_BLUE_PIN)
#define LED_BLUE_TOGGLE     (NRF_GPIO->OUT ^= LED_BLUE_PIN)
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
