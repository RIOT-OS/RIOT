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
#define CC110X_PARAM_SPI        SPI_DEV(0)              /**< SPI interface CC1101 is connected to */
#define CC110X_PARAM_CS         GPIO_PIN(PORT_B, 12)    /**< CS pin of CC1101 */
#define CC110X_PARAM_GDO0       GPIO_PIN(PORT_C, 4)     /**< GDO0 pin of CC1101 */
#define CC110X_PARAM_GDO2       GPIO_PIN(PORT_C, 5)     /**< GDO2 pin of CC1101 */
#define CC110X_PARAM_SPI_CLOCK  SPI_CLK_1MHZ            /**< SPI clock (reduced to work around hw bug) */
/** @} */

/**
 * @name    Configure connected MPU-9150 device
 * @{
 */
#define MPU9X50_PARAM_COMP_ADDR   (0x0E)            /**< I2C address of the MPU9150 */
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 8)     /**< Pin of red LED */
#define LED1_PIN            GPIO_PIN(PORT_B, 14)    /**< Pin of yellow LED */
#define LED2_PIN            GPIO_PIN(PORT_B, 15)    /**< Pin of green LED */

#define LED_PORT            GPIOB       /**< GPIO port LEDs are connected to */
#define LED0_MASK           (1 << 8)    /**< Bitmask to address red LED in @ref LED_PORT */
#define LED1_MASK           (1 << 14)   /**< Bitmask to address yellow LED in @ref LED_PORT */
#define LED2_MASK           (1 << 15)   /**< Bitmask to address green LED in @ref LED_PORT */

#define LED0_ON             (LED_PORT->BSRR = LED0_MASK)            /**< Turn red LED on */
#define LED0_OFF            (LED_PORT->BSRR = (LED0_MASK << 16))    /**< Turn red LED off */
#define LED0_TOGGLE         (LED_PORT->ODR  ^= LED0_MASK)           /**< Toggle red LED */

#define LED1_ON             (LED_PORT->BSRR = LED1_MASK)            /**< Turn yellow LED on */
#define LED1_OFF            (LED_PORT->BSRR = (LED1_MASK << 16))    /**< Turn yellow LED off */
#define LED1_TOGGLE         (LED_PORT->ODR  ^= LED1_MASK)           /**< Toggle yellow LED */

#define LED2_ON             (LED_PORT->BSRR = LED2_MASK)            /**< Turn green LED on */
#define LED2_OFF            (LED_PORT->BSRR = (LED2_MASK << 16))    /**< Turn green LED off */
#define LED2_TOGGLE         (LED_PORT->ODR  ^= LED2_MASK)           /**< Toggle green LED */
/** @} */

/**
 * @name    Button pin definitions
 * @{
 */
#define BUTTON0_PIN         GPIO_PIN(PORT_B, 13)    /**< Pin of left button */
#define BUTTON1_PIN         GPIO_PIN(PORT_A, 0)     /**< Pin of right button */
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
