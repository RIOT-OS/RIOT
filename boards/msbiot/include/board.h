/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_msbiot MSB-IoT
 * @ingroup     boards
 * @brief       Board specific files for the MSB-IoT board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the MSB-IoT board
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Assign the hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Configure connected CC1101 (radio) device
 * @{
 */
#define CC110X_SPI          SPI_0
#define CC110X_CS           GPIO_7
#define CC110X_GDO0         GPIO_11
#define CC110X_GDO1         GPIO_2
#define CC110X_GDO2         GPIO_12

typedef uint8_t radio_packet_length_t;
/** @} */

/**
 * @name Define UART device and baudrate for stdio
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200U)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            GPIOB
#define LED_RED_PIN         (1 << 8)
#define LED_YELLOW_PIN      (1 << 14)
#define LED_GREEN_PIN       (1 << 15)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_PORT->BSRRH = LED_RED_PIN)
#define LED_RED_OFF         (LED_PORT->BSRRL = LED_RED_PIN)
#define LED_RED_TOGGLE      (LED_PORT->ODR ^= LED_RED_PIN)
#define LED_YELLOW_ON       (LED_PORT->BSRRH = LED_YELLOW_PIN)
#define LED_YELLOW_OFF      (LED_PORT->BSRRL = LED_YELLOW_PIN)
#define LED_YELLOW_TOGGLE   (LED_PORT->ODR ^= LED_YELLOW_PIN)
#define LED_GREEN_ON        (LED_PORT->BSRRH = LED_GREEN_PIN)
#define LED_GREEN_OFF       (LED_PORT->BSRRL = LED_GREEN_PIN)
#define LED_GREEN_TOGGLE    (LED_PORT->ODR ^= LED_GREEN_PIN)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /** __BOARD_H */
/** @} */
