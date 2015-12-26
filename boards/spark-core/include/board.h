/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_spark-core Spark-Core
 * @ingroup     boards
 * @brief       Board specific files for the spark-core board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the spark-core board.
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef BOARD_H_
#define BOARD_H_

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Define the nominal CPU core clock in this board
 */
#define F_CPU               CLOCK_CORECLOCK

/**
 * @name Define the location of the RIOT image in flash
 */
#define LOCATION_VTABLE     (0x08005000)

/**
 * @name Define the UART to be used as stdio and its baudrate
 * @{
 */
#define STDIO               UART_0
#define STDIO_BAUDRATE      (115200)
#define STDIO_RX_BUFSIZE    (64U)
/** @} */

/**
 * @name LED pin definitions
 * @{
 */
#define LED_PORT            (GPIOA)
#define LED_RED_PIN         (9)
#define LED_GREEN_PIN       (10)
#define LED_BLUE_PIN        (8)
#define LED_WHITE_PIN       (13)
/** @} */

/**
 * @name Macros for controlling the on-board LEDs.
 * @{
 */
#define LED_RED_ON          (LED_PORT->BRR = (1<<LED_RED_PIN))
#define LED_RED_OFF         (LED_PORT->BSRR = (1<<LED_RED_PIN))
#define LED_RED_TOGGLE      (LED_PORT->ODR ^= (1<<LED_RED_PIN))
#define LED_GREEN_ON        (LED_PORT->BRR = (1<<LED_GREEN_PIN))
#define LED_GREEN_OFF       (LED_PORT->BSRR = (1<<LED_GREEN_PIN))
#define LED_GREEN_TOGGLE    (LED_PORT->ODR ^= (1<<LED_GREEN_PIN))
#define LED_BLUE_ON         (LED_PORT->BRR = (1<<LED_BLUE_PIN))
#define LED_BLUE_OFF        (LED_PORT->BSRR = (1<<LED_BLUE_PIN))
#define LED_BLUE_TOGGLE     (LED_PORT->ODR ^= (1<<LED_BLUE_PIN))
#define LED_WHITE_ON        (LED_PORT->BRR = (1<<LED_WHITE_PIN))
#define LED_WHITE_OFF       (LED_PORT->BSRR = (1<<LED_WHITE_PIN))
#define LED_WHITE_TOGGLE    (LED_PORT->ODR ^= (1<<LED_WHITE_PIN))
/** @} */

/**
 * @name User button configuration
 */
#define BUTTON1             GPIO_PIN(PORT_B,2)

/**
 * @name CC3000 pin configuration
 * @{
 */
#define CC3000_SPI          SPI_0
#define CC3000_CS           GPIO_PIN(PORT_B,12)
#define CC3000_EN           GPIO_PIN(PORT_B,8)
#define CC3000_INT          GPIO_PIN(PORT_B,11)
/** @} */

/**
 * @name EXTFLASH pin configuration
 * @{
 */
#define EXTFLASH_SPI        SPI_0
#define EXTFLASH            GPIO_PIN(PORT_B,9)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H_ */
/** @} */
