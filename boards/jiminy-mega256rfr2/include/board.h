/*
 * Copyright (C) 2016 RWTH Aachen, Josua Arndt
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_jiminy-mega256rfr2
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Jiminy Mega 256rfr2 board.
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Steffen Robertz <steffen.robertz@rwth-aachen.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Baudrate for STDIO terminal
 *
 * The standard configuration for STDIO in spu/atmega_comman/periph/uart.c
 * is to use double speed.
 *
 * For 8MHz F_CPU following Baudrate have good error rates
 *  76923
 *  38400
 *
 * Matches this with BAUD in Board/Makefile.include
 *
 * @{
 */
#ifndef STDIO_UART_BAUDRATE
#define STDIO_UART_BAUDRATE (38400U)       /**< Sets Baudrate for e.g. Shell */
#endif
/** @} */

/**
 * @name   LED pin definitions and handlers
 * @{
 */
#define LED_PORT            PORTB
#define LED_PORT_DDR        DDRB

#define LED0_PIN            GPIO_PIN(1, 5)
#define LED1_PIN            GPIO_PIN(1, 6)
#define LED2_PIN            GPIO_PIN(1, 7)

#define LED0_MASK           (1 << DDB5)
#define LED1_MASK           (1 << DDB6)
#define LED2_MASK           (1 << DDB7)

#define LED0_ON             (LED_PORT |=  LED0_MASK)
#define LED0_OFF            (LED_PORT &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT ^=  LED0_MASK)

#define LED1_ON             (LED_PORT |=  LED1_MASK)
#define LED1_OFF            (LED_PORT &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT ^=  LED1_MASK)

#define LED2_ON             (LED_PORT |=  LED2_MASK)
#define LED2_OFF            (LED_PORT &= ~LED2_MASK)
#define LED2_TOGGLE         (LED_PORT ^=  LED2_MASK)
/** @} */

/**
 * @name   White LED light is used to signal ERROR.
 * @{
 */
#define LED_PANIC           (LED_PORT |= LED2_MASK | LED1_MASK | LED0_MASK)
/** @} */

/**
 * @name xtimer configuration values
 * @{
 */
#define XTIMER_DEV          TIMER_DEV(0)
#define XTIMER_CHAN         (0)
#define XTIMER_WIDTH        (16)
#define XTIMER_HZ           (125000UL)
/** @} */

/**
 * @name Indicate Watchdog cleared in bootloader an
 *
 * AVR CPUs need to reset the Watchdog as fast as possible.
 * This flag indicates that the watchdog is reseted in the bootloader
 * and that the MCUSR value is stored in register 2 (r2)
 * @{
 */
#define BOOTLOADER_CLEARS_WATCHDOG_AND_PASSES_MCUSR 1
/** @} */

/**
 * @name Indicate Watchdog cleared in bootloader an
 *
 * AVR CPUs need to reset the Watchdog as fast as possible.
 * This flag indicates that the watchdog is reseted in the bootloader
 * and that the MCUSR value is stored in register 2 (r2)
 * @{
 */
#define BOOTLOADER_CLEARS_WATCHDOG_AND_PASSES_MCUSR 1
/** @} */

/**
 * @name CPU clock scale for jiminy-megarfr256rfr2
 *
 * The CPU can not be used with the external xtal oscillator if the core
 * should be put in sleep while the transceiver is in rx mode.
 *
 * It seems the as teh peripheral clock divider is set to 1 and this all
 * clocks of the timer, etc run with 16MHz increasing power consumption.
 */
#define CPU_ATMEGA_CLK_SCALE_INIT    CPU_ATMEGA_CLK_SCALE_DIV1
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
