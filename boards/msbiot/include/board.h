/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_msbiot
 * @{
 *
 * @file
 * @brief       Board specific definitions for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Configure connected CC1101 (radio) device
 * @{
 */
#define CC110X_PARAM_SPI    SPI_DEV(0)
#define CC110X_PARAM_CS     GPIO_PIN(PORT_B, 12)
#define CC110X_PARAM_GDO0   GPIO_PIN(PORT_C, 4)
#define CC110X_PARAM_GDO1   GPIO_PIN(PORT_A, 6)
#define CC110X_PARAM_GDO2   GPIO_PIN(PORT_C, 5)
/** @} */

/**
 * @name    Configure connected MPU-9150 device
 * @{
 */
#define MPU9150_PARAM_COMP_ADDR   (0x0E)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 8)
#define LED1_PIN            GPIO_PIN(PORT_B, 14)
#define LED2_PIN            GPIO_PIN(PORT_B, 15)

#define LED_PORT            GPIOB
#define LED0_MASK           (1 << 8)
#define LED1_MASK           (1 << 14)
#define LED2_MASK           (1 << 15)

#define LED0_ON             (LED_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED_PORT->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (LED_PORT->ODR  ^= LED0_MASK)

#define LED1_ON             (LED_PORT->BSRR = LED1_MASK)
#define LED1_OFF            (LED_PORT->BSRR = (LED1_MASK << 16))
#define LED1_TOGGLE         (LED_PORT->ODR  ^= LED1_MASK)

#define LED2_ON             (LED_PORT->BSRR = LED2_MASK)
#define LED2_OFF            (LED_PORT->BSRR = (LED2_MASK << 16))
#define LED2_TOGGLE         (LED_PORT->ODR  ^= LED2_MASK)
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
#define BUTTON0_PIN         GPIO_PIN(PORT_B, 13)
#define BUTTON1_PIN         GPIO_PIN(PORT_A, 0)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
