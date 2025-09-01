/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_atmega256rfr2-xpro
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmega256RFR2 Xplained Pro
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use the UART 1 for STDIO on this board
 */
#define STDIO_UART_DEV       (UART_DEV(1))

/**
 * @name    xtimer configuration values
 *
 * Xtimer runs at 16MHz / 64 = 250kHz
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
#define CONFIG_ZTIMER_USEC_ADJUST_SET     (132)
#define CONFIG_ZTIMER_USEC_ADJUST_SLEEP   (120)
/** @} */

/**
 * @name    Macros for controlling the on-board LED
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 4)
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
#define BTN0_PIN     GPIO_PIN(PORT_E, 4)
#define BTN0_MODE    GPIO_IN_PU
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
