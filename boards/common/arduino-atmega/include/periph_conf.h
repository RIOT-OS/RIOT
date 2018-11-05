/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2017 HAW Hamburg, Dimitri Nahm
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_arduino-atmega
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for Arduino Atmega boards
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Dimitri Nahm <dimitri.nahm@haw-hamburg.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (16000000UL)
/** @} */

/**
 * @name    Timer configuration
 *
 * arduino uno and arduino duemilanove:
 * The timer driver only supports the 16-bit timer (Timer1)
 * so this is the only one we can use here.
 *
 * arduino mega2560:
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only onces we can use here.
 *
 * @{
 */
#ifdef CPU_ATMEGA328P
#define TIMER_NUMOF         (2U)

#define TIMER_0             MEGA_TIMER1
#define TIMER_0_MASK        &TIMSK1
#define TIMER_0_FLAG        &TIFR1
#define TIMER_0_ISRA        TIMER1_COMPA_vect
#define TIMER_0_ISRB        TIMER1_COMPB_vect
#endif

#ifdef CPU_ATMEGA2560
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
#endif
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
#ifdef CPU_ATMEGA328P
#define UART_NUMOF          (1U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART_RX_vect
#endif

#ifdef CPU_ATMEGA2560
#define UART_NUMOF          (4U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect

#define UART_2              MEGA_UART2
#define UART_2_ISR          USART2_RX_vect

#define UART_3              MEGA_UART3
#define UART_3_ISR          USART3_RX_vect
#endif
/** @} */

/**
 * @name    SPI configuration
 *
 * The atmega2560/atmega1281/atmega328p has only one hardware SPI with fixed pin
 * configuration, so all we can do here, is to enable or disable it...
 *
 * The fixed pins for arduino uno and duemilanove (atmega328p) are:
 * MOSI - PB3 (Arduino pin 11)
 * MISO - PB4 (Arduino pin 12)
 * SCK  - PB5 (Arduino pin 13)
 * SS   - PB2 (Arduino pin 10) -> this pin is configured as output, but not used
 *
 * The fixed pins for arduino-mega2560 and atmega1281 are:
 * MOSI - PB2 (Arduino pin 51)
 * MISO - PB3 (Arduino pin 50)
 * SCK  - PB1 (Arduino pin 52)
 * SS   - PB0 (Arduino pin 53) -> this pin is configured as output, but not used
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
 *
 * The number of ADC channels of the atmega328p depends on the package:
 *  - 6-channel 10-bit ADC in PDIP package
 *  - 8-channel 10-bit ADC in TQFP and QFN/MLF package
 * Arduino UNO / Duemilanove has thereby 6 channels. But only 5 channels can be
 * used for ADC, because the pin of ADC5 emulate a software triggered interrupt.
 *  -> boards -> arduino-atmega-common -> include -> board_common.h
 * @{
 */
#if defined (CPU_ATMEGA328P) || defined (CPU_ATMEGA1281)
#define ADC_NUMOF       (8U)
#elif defined (CPU_ATMEGA2560)
#define ADC_NUMOF       (16U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
