/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2019 Otto-von-Guericke-Universität Magdeburg
 *               2021-2123 Gerson Fernando Budke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     boards_atmega328p_xplained_mini
 * @{
 *
 * @file
 * @brief       Board specific definitions for the standalone ATmega328p "board"
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#include "cpu.h"
#include "periph_conf.h"

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
#define STDIO_UART_BAUDRATE         (9600U)
/** @} */

/**
 * @name    xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH                (16)
#if CLOCK_CORECLOCK > 4000000UL
#define XTIMER_HZ                   (CLOCK_CORECLOCK / 64)
#else
#define XTIMER_HZ                   (CLOCK_CORECLOCK / 8)
#endif
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    Macros for controlling the on-board LED
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 5)
#define LED0_MODE           GPIO_OUT
#define LED0_ENABLE_PORT    DDRB  |=  LED0_PIN
#define LED0_ON             PORTB |=  LED0_PIN
#define LED0_OFF            PORTB &= ~LED0_PIN
#define LED0_TOGGLE         PORTB ^=  LED0_PIN
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_B, 7)
#define BTN0_MODE           GPIO_IN_PU
#define BTN0_INT_FLANK      GPIO_FALLING
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
