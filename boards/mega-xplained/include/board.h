/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup     boards_mega-xplained
 * @brief       Support for the Mega Xplained board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Mega Xplained board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_cpu.h"
#include "mega-xplained_pinmap.h"

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
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (9600U)
#endif
/** @} */

/**
 * @brief   Use the UART 1 for STDIO on this board
 */
#define STDIO_UART_DEV       (UART_DEV(1))

/**
 * @name    xtimer configuration values
 *
 * Xtimer runs at 8MHz / 64 = 125kHz
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_HZ                   (125000UL)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED pin definitions
 * @{
 */
/* LED0,2 currently unsupported due to lack of GPIO_OD support */
#define LED1_PIN     GPIO_PIN(PORT_B, 3)
#define LED1_MODE    GPIO_OUT

#define LED3_PIN     GPIO_PIN(PORT_B, 2)
#define LED3_MODE    GPIO_OUT
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
/* LED0,2 currently unsupported due to lack of GPIO_OD support */
#define LED1_ENABLE_PORT    DDRB  |=  LED1_PIN
#define LED1_ON             PORTB |=  LED1_PIN
#define LED1_OFF            PORTB &= ~LED1_PIN
#define LED1_TOGGLE         PORTB ^=  LED1_PIN

#define LED3_ENABLE_PORT    DDRB  |=  LED3_PIN
#define LED3_ON             PORTB |=  LED3_PIN
#define LED3_OFF            PORTB &= ~LED3_PIN
#define LED3_TOGGLE         PORTB ^=  LED3_PIN
/** @} */

/**
 * @name    Usage of LED to turn on when a kernel panic occurs.
 * @{
 */
#define LED_PANIC           LED1_ON
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN     GPIO_PIN(PORT_B, 0)
#define BTN0_MODE    GPIO_IN

#define BTN1_PIN     GPIO_PIN(PORT_B, 1)
#define BTN1_MODE    GPIO_IN

/* BTN2 currently unsupported due to lack of GPIO_OD support */

/**
 * @name    ADC NTC, light sensor, and filter lines
 * @{
 */
#define NTC_OUTPUT             GPIO_PIN(PORT_A, 5)
#define LIGHT_SENSOR_OUTPUT    GPIO_PIN(PORT_A, 6)
#define FILTER_OUTPUT          GPIO_PIN(PORT_A, 7)
/** @} */

/**
 * @name CPU clock scale for mega-xplained
 *
 */
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
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
