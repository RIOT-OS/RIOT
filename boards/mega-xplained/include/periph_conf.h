/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_mega-xplained
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for Mega Xplained
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 *
 * Frequency of the internal 8MHz RC oscillator.
 *
 * @{
 */
#define CLOCK_CORECLOCK     (8000000UL)
/** @} */

/**
 * @name    Timer configuration
 *
 * The ATmega1284P has 4 timers. Timer0 and Timer2 are 8 Bit Timers.
 *
 * The timer driver only supports the two 16-bit timers (Timer1 and
 * Timer3), so those are the only ones we can use here.
 *
 * @{
 */
#define TIMER_NUMOF         (2U)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect

#define TIMER_1             MEGA_TIMER3
#define TIMER_1_MASK        &TIMSK3
#define TIMER_1_FLAG        &TIFR3
#define TIMER_1_ISRA        TIMER3_COMPA_vect
#define TIMER_1_ISRB        TIMER3_COMPB_vect
/** @} */

/**
 * @name    UART configuration
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
/** @} */

/**
 * @name    SPI configuration
 *
 * The ATmega1284P has only one hardware SPI with fixed pin configuration, so
 * all we can do here, is to enable or disable it...
 *
 * The fixed pins used are:
 * MOSI - PB5 (pin  1)
 * MISO - PB6 (pin  2)
 * SCK  - PB7 (pin  3)
 * SS   - PB4 (pin 44) -> this pin is configured as output, but not used
 *
 * The SS pin must be configured as output for the SPI device to work as
 * master correctly, though we do not use it for now (as we handle the chip
 * select externally for now)
 *
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#define I2C_BUS_SPEED       I2C_SPEED_FAST
#define I2C_NUMOF           1
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
#define ADC_NUMOF       (8U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
