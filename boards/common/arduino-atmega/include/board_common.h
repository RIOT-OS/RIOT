/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @defgroup    boards_common_arduino-atmega Arduino Atmega Common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for Arduino Atmega boards.
 * @{
 *
 * @file
 * @brief       Common board configuration for Arduino Atmega boards
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    STDIO configuration
 *
 * As the CPU is too slow to handle 115200 baud, we set the default
 * baudrate to 9600 for this board
 * @{
 */
#define STDIO_UART_BAUDRATE (9600U)
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#ifdef CPU_ATMEGA328P
#define LED0_PIN            GPIO_PIN(1, 5)
#define LED0_MASK           (1 << DDB5)
#endif

#ifdef CPU_ATMEGA32U4
#define LED0_PIN            GPIO_PIN(2, 7) /**< BUILTIN LED */
#define LED0_MASK           (1 << DDC7)
#define LED1_PIN            GPIO_PIN(1, 0) /**< RX LED */
#define LED1_MASK           (1 << DDB0)
#define LED2_PIN            GPIO_PIN(3, 5) /**< TX LED */
#define LED2_MASK           (1 << DDD5)
#endif

#ifdef CPU_ATMEGA2560
#define LED0_PIN            GPIO_PIN(1, 7)
#define LED0_MASK           (1 << DDB7)
#endif

#ifdef CPU_ATMEGA32U4
#define LED0_ON             (PORTC |=  LED0_MASK) /**< BUILTIN LED */
#define LED0_OFF            (PORTC &= ~LED0_MASK)
#define LED0_TOGGLE         (PORTC ^=  LED0_MASK)
#define LED1_OFF            (PORTB |=  LED1_MASK) /**< RX LED */
#define LED1_ON             (PORTB &= ~LED1_MASK)
#define LED1_TOGGLE         (PORTB ^=  LED1_MASK)
#define LED2_OFF            (PORTD |=  LED2_MASK) /**< TX LED */
#define LED2_ON             (PORTD &= ~LED2_MASK)
#define LED2_TOGGLE         (PORTD ^=  LED2_MASK)
#else
#define LED0_ON             (PORTD |=  LED0_MASK)
#define LED0_OFF            (PORTD &= ~LED0_MASK)
#define LED0_TOGGLE         (PORTD ^=  LED0_MASK)
#endif
/** @} */

/**
 * @name    Usage of LED to turn on when a kernel panic occurs.
 * @{
 */
#define LED_PANIC           LED0_ON
/** @} */

/**
 * @name CPU clock scale for arduino boards
 *
 */
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
/** @} */

/**
 * @name    xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH                (16)
#define XTIMER_HZ                   (250000UL)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    ztimer configuration values
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE     ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV      (TIMER_DEV(0))
#define CONFIG_ZTIMER_USEC_FREQ     (250000LU)
#define CONFIG_ZTIMER_USEC_WIDTH    (16)
/** @} */

/**
 * @name    Configuration parameters for the W5100 driver
 * @{
 */
#ifndef W5100_PARAM_CS
#define W5100_PARAM_CS          (ARDUINO_PIN_10)
#endif
#ifndef W5100_PARAM_EVT
#define W5100_PARAM_EVT         (ARDUINO_PIN_2)
#endif
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
