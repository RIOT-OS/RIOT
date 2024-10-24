/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k22f
 * @{
 *
 * @file
 * @brief       Board specific definitions for the FRDM-K22F
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef BOARD_H
#define BOARD_H

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
#define LED0_PIN            GPIO_PIN(PORT_A,  1)
#define LED1_PIN            GPIO_PIN(PORT_A,  2)
#define LED2_PIN            GPIO_PIN(PORT_D,  5)

#define LED0_MASK           (1 <<  1)
#define LED1_MASK           (1 <<  2)
#define LED2_MASK           (1 <<  5)

#define LED0_ON            (GPIOA->PCOR = LED0_MASK)
#define LED0_OFF           (GPIOA->PSOR = LED0_MASK)
#define LED0_TOGGLE        (GPIOA->PTOR = LED0_MASK)

#define LED1_ON            (GPIOA->PCOR = LED1_MASK)
#define LED1_OFF           (GPIOA->PSOR = LED1_MASK)
#define LED1_TOGGLE        (GPIOA->PTOR = LED1_MASK)

#define LED2_ON            (GPIOD->PCOR = LED2_MASK)
#define LED2_OFF           (GPIOD->PSOR = LED2_MASK)
#define LED2_TOGGLE        (GPIOD->PTOR = LED2_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
/* SW2, SW3 will short these pins to ground when pushed. Both pins have external
 * pull-up resistors to VDD */
/* BTN0 is mapped to SW2 */
#define BTN0_PIN            GPIO_PIN(PORT_C,  1)
#define BTN0_MODE           GPIO_IN
/* BTN1 is mapped to SW3 */
#define BTN1_PIN            GPIO_PIN(PORT_B, 17)
#define BTN1_MODE           GPIO_IN
/** @} */

/**
 * @name    FXOS8700CQ 3-axis accelerometer and magnetometer bus configuration
 * @{
 */
#define FXOS8700_PARAM_I2C          I2C_DEV(0)
#define FXOS8700_PARAM_ADDR         0x1C
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* BOARD_H */
