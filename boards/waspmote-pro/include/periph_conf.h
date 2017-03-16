/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 * Copyright (C) 2016 INRIA, Francisco Acosta
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_waspmote-pro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Waspmote PRO v1.2 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (14745600L)
/** @} */

/**
 * @name   Timer peripheral configuration
 *
 * The ATmega1281 has 6 timers. Timer0 and Timer2 are 8 Bit Timers,
 * Timer0 has special uses too and therefore we'll avoid using it.
 *
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only ones we can use here.
 *
 * @{
 */
#define TIMER_NUMOF         (2U)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#define TIMER_0_ISRC        TIMER1_COMPC_vect

#define TIMER_1             MEGA_TIMER4
#define TIMER_1_MASK        &TIMSK4
#define TIMER_1_FLAG        &TIFR4
#define TIMER_1_ISRA        TIMER4_COMPA_vect
#define TIMER_1_ISRB        TIMER4_COMPB_vect
#define TIMER_1_ISRC        TIMER4_COMPC_vect
/** @} */


/**
 * @name   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (2U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect

/*
 * UART speed constants for CLK=14745600
 */
#define UART_2400           (383)
#define UART_4800           (191)
#define UART_9600           (95)
#define UART_115200         (7)

/** @} */

/**
 * @name   SPI configuration
 *
 * The atmega1281 has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 *
 * The fixed pins used, are:
 * MOSI - PB2 (waspmote SPI-UART socket MOSI)
 * MISO - PB3 (waspmote SPI-UART socket MISO)
 * SCK  - PB1 (waspmote SPI-UART socket SCK)
 * SS   - PB0 (waspmote SD Card Chip Select) -> this pin is configured as output, but not used
 *
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
#define MEGA_PRR            PRR0        /* Power Reduction Register */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
