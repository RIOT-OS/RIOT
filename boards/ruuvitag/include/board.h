/*
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_ruuvitag
 * @{
 *
 * @file
 * @brief       Board specific configuration for the RuuviTag board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin configuration
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 17)
#define LED1_PIN            GPIO_PIN(0, 19)

/* as a shortcut, we include the two CS pins into the LED_MASK, this will make
 * sure both pins are initialized as output and set during board initialization.
 * This is needed so that the BMX280 and LIS2DH12 device drivers do not
 * interfere... */
#define CS_PIN_MASK         ((1 << 3) | (1 << 8))

#define LED_PORT            NRF_P0
#define LED0_MASK           (1 << 17)
#define LED1_MASK           (1 << 19)

/* mask used in the common board.c file to initialize and set the given pins */
#define LED_MASK            (LED0_MASK | LED1_MASK | CS_PIN_MASK)

#define LED0_ON             (LED_PORT->OUTCLR = LED0_MASK)
#define LED0_OFF            (LED_PORT->OUTSET = LED0_MASK)
#define LED0_TOGGLE         (LED_PORT->OUT   ^= LED0_MASK)

#define LED1_ON             (LED_PORT->OUTCLR = LED1_MASK)
#define LED1_OFF            (LED_PORT->OUTSET = LED1_MASK)
#define LED1_TOGGLE         (LED_PORT->OUT   ^= LED1_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 13)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Environmental sensor configuration (Bosch BMX280)
 * @{
 */
#define BMX280_PARAM_CS     GPIO_PIN(0, 3)
/** @} */

/**
 * @name    Accelerometer configuration
 * @{
 */
#define LIS2DH12_PARAM_CS       GPIO_PIN(0, 8)
#define LIS2DH12_PARAM_INT_PIN1 GPIO_PIN(0, 2)
#define LIS2DH12_PARAM_INT_PIN2 GPIO_PIN(0, 6)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
