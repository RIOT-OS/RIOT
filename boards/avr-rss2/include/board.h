/*
 * Copyright (C) 2019 Robert Olsson <roolss@kth.se>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_avr-rss2
 * @{
 *
 * @file
 * @brief       Board definitions for the rss2 256rfr2 board.
 *
 * @author      Robert Olsson <roolss@kth.se>
 *
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORTE
#define LED_PORT_DDR        DDRE

#define LED0_PIN            GPIO_PIN(PORT_E, 4) /* RED */
#define LED1_PIN            GPIO_PIN(PORT_E, 3) /* YELLOW */

#define LED0_MASK           (1 << 4)
#define LED1_MASK           (1 << 3)

#define LED0_MODE           GPIO_OUT
#define LED0_OFF            (LED_PORT |=  LED0_MASK)
#define LED0_ON             (LED_PORT &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT ^=  LED0_MASK)

#define LED1_MODE           GPIO_OUT
#define LED1_OFF            (LED_PORT |=  LED1_MASK)
#define LED1_ON             (LED_PORT &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT ^=  LED1_MASK)


/**
 * @name    Usage of LED to turn on when a kernel panic occurs.
 * @{
 */
#define LED_PANIC           LED0_ON
/** @} */


/**
 * @name DS18 pins  OW_BUS_0
 * @{
 */
#define DS18_PARAM_PIN      GPIO_PIN(PORT_D, 7)
#define DS18_PARAM_PULL     (GPIO_IN_PU)

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (62500UL)

/**
 * @name Indicate Watchdog cleared in bootloader an
 *
 * AVR CPUs need to reset the Watchdog as fast as possible.
 * This flag indicates that the watchdog is reset in the bootloader
 * and that the MCUSR value is stored in register 0 (r0)
 * @{
 */
#define BOOTLOADER_CLEARS_WATCHDOG_AND_PASSES_MCUSR 0
/** @} */

/**
 * @name CPU clock scale for avr-rss2
 *
 */
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
/** @} */

/**
 * @name    User button configuration
 * @{
 */
#define MODULE_ATMEGA_PCINT0
#define BTN0_PIN            GPIO_PIN(PORT_B, 0)
#define BTN0_MODE           GPIO_IN
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
