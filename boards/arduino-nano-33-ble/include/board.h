/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-nano-33-ble
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Arduino Nano 33 BLE
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LEDs pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 13)
#define LED0_MASK           (1 << 13)
#define LED0_ON             (NRF_P0->OUTCLR = LED0_MASK)
#define LED0_OFF            (NRF_P0->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (NRF_P0->OUT   ^= LED0_MASK)

#define LED1_PIN            GPIO_PIN(0, 24)
#define LED1_MASK           (1 << 24)
#define LED1_ON             (NRF_P0->OUTCLR = LED1_MASK)
#define LED1_OFF            (NRF_P0->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (NRF_P0->OUT   ^= LED1_MASK)

#define LED2_PIN            GPIO_PIN(0, 16)
#define LED2_MASK           (1 << 16)
#define LED2_ON             (NRF_P0->OUTCLR = LED2_MASK)
#define LED2_OFF            (NRF_P0->OUTSET = LED2_MASK)
#define LED2_TOGGLE         (NRF_P0->OUT   ^= LED2_MASK)

#define LED3_PIN            GPIO_PIN(0, 6)
#define LED3_MASK           (1 << 6)
#define LED3_ON             (NRF_P0->OUTCLR = LED3_MASK)
#define LED3_OFF            (NRF_P0->OUTSET = LED3_MASK)
#define LED3_TOGGLE         (NRF_P0->OUT   ^= LED3_MASK)

#define LED4_PIN            GPIO_PIN(1, 9)
#define LED4_MASK           (1 << 9)
#define LED4_ON             (NRF_P1->OUTCLR = LED4_MASK)
#define LED4_OFF            (NRF_P1->OUTSET = LED4_MASK)
#define LED4_TOGGLE         (NRF_P1->OUT   ^= LED4_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
