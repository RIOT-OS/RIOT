/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 PHYTEC Messtechnik GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_frdm-k64f
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-K64F
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 22)
#define LED1_PIN            GPIO_PIN(PORT_E, 26)
#define LED2_PIN            GPIO_PIN(PORT_B, 21)

#define LED0_MASK           (1 << 22)
#define LED1_MASK           (1 << 26)
#define LED2_MASK           (1 << 21)

#define LED0_ON            (GPIOB->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOB->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOB->PTOR = LED0_MASK)

#define LED1_ON            (GPIOE->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOE->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOE->PTOR = LED1_MASK)

#define LED2_ON            (GPIOB->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOB->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOB->PTOR = LED2_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* SW2, SW3 will short these pins to ground when pushed. PTA4 has an external
 * pull-up resistor to VDD, but there is no external pull resistor on PTC6 */
/* BTN0 is mapped to SW2 */
#define BTN0_PIN            GPIO_PIN(PORT_C,  6)
#define BTN0_MODE           GPIO_IN_PU
/* BTN1 is mapped to SW3 */
#define BTN1_PIN            GPIO_PIN(PORT_A,  4)
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    FXOS8700CQ 3-axis accelerometer and magnetometer bus configuration
 * @{
 */
#define FXOS8700_PARAM_I2C          I2C_DEV(0)
#define FXOS8700_PARAM_ADDR         0x1E
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
