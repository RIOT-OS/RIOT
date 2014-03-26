/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_stm32f3discovery STM32F3Discovery
 * @ingroup     boards
 * @brief       Board specific files for the STM32F3Discovery board.
 * @{
 *
 * @file        board.h
 * @brief       Board specific definitions for the STM32F3Discovery evaluation board.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "stm32f30x.h"

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               (72000000UL)


/**
 * @name Assign the hardware timer
 * @{
 */
#define HW_TIMER            TIMER_0
/** @} */

/**
 * @name Example: configure connected NRF24L01+ (radio) device
 * 
 * This is just an example on how to bind (externally or internally) connected
 * devices to the CPUs peripherals.
 * @{
 */
#define NRF24L01P_NUMOF     (1U)
#define NRF24L01P_SPI       SPI_0
#define NRF24L01P_CE        GPIO_0
#define NRF24L01P_CSN       GPIO_1
#define NRF24L01P_INT       GPIO_2
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOE
#define LD3_PIN             (1 << 9)
#define LD4_PIN             (1 << 8)
#define LD5_PIN             (1 << 10)
#define LD6_PIN             (1 << 15)
#define LD7_PIN             (1 << 11)
#define LD8_PIN             (1 << 14)
#define LD9_PIN             (1 << 12)
#define LD10_PIN            (1 << 13)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LD3_ON              LED_PORT->BSRR = LD3_PIN
#define LD3_OFF             LED_PORT->BRR = LD3_PIN
#define LD4_ON              LED_PORT->BSRR = LD4_PIN
#define LD4_OFF             LED_PORT->BRR = LD4_PIN
#define LD5_ON              LED_PORT->BSRR = LD5_PIN
#define LD5_OFF             LED_PORT->BRR = LD5_PIN
#define LD6_ON              LED_PORT->BSRR = LD6_PIN
#define LD6_OFF             LED_PORT->BRR = LD6_PIN
#define LD7_ON              LED_PORT->BSRR = LD7_PIN
#define LD7_OFF             LED_PORT->BRR = LD7_PIN
#define LD8_ON              LED_PORT->BSRR = LD8_PIN
#define LD8_OFF             LED_PORT->BRR = LD8_PIN
#define LD9_ON              LED_PORT->BSRR = LD9_PIN
#define LD9_OFF             LED_PORT->BRR = LD9_PIN
#define LD10_ON             LED_PORT->BSRR = LD10_PIN
#define LD10_OFF            LED_PORT->BRR = LD10_PIN

/* for compatability to other boards */
#define LED_GREEN_ON        LD3_ON
#define LED_GREEN_OFF       LD3_OFF
#define LED_RED_ON          LD10_ON
#define LED_RED_OFF         LD10_OFF
/** @} */


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);


/** @} */
#endif /** __BOARD_H */
