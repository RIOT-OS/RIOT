/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_arduino-mega2560
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Arduino Mega 2560 board
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H_
#define PERIPH_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 * @{
 */
#define CLOCK_CORECLOCK     (16000000L)
/** @} */

/**
 * @brief   Timer configuration
 *
 * The timer driver only supports the four 16-bit timers (Timer1, Timer3,
 * Timer4, Timer5), so those are the only onces we can use here.
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
 * @brief   UART configuration
 * @{
 */
#define UART_NUMOF          (4U)
#define UART_0_EN           1
#define UART_1_EN           1
#define UART_2_EN           1
#define UART_3_EN           1

/* UART 0 registers */
#define UART0_CTRL_STAT_A   UCSR0A
#define UART0_CTRL_STAT_B   UCSR0B
#define UART0_CTRL_STAT_C   UCSR0C
#define UART0_BAUD_RATE_L   UBRR0L
#define UART0_BAUD_RATE_H   UBRR0H
#define UART0_DATA_REGISTER UDR0

/* Flags */
#define UART0_RX_COMPLETE   RXC0
#define UART0_DATA_EMPTY    UDRE0
#define UART0_RX_EN         RXEN0
#define UART0_TX_EN         TXEN0
#define UART0_RXC_IRQ_EN    RXCIE0
#define UART0_TXC_IRQ_EN    TXCIE0
#define UART0_8BIT_SIZE     (1 << UCSZ00) | (1 << UCSZ01)

/* UART0 helper macros */
#define UART0_RX_TX_EN      UART0_CTRL_STAT_B |= (1 << UART0_RX_EN) | (1 << UART0_TX_EN)
#define UART0_RX_IRQ_EN     UART0_CTRL_STAT_B |= (1 << UART0_RXC_IRQ_EN)
#define UART0_SET_8BIT_SIZE UART0_CTRL_STAT_C |= UART0_8BIT_SIZE
#define UART0_RECEIVED_DATA (UART0_CTRL_STAT_A & (1 << UART0_RX_COMPLETE))
#define UART0_DTREG_EMPTY   (UART0_CTRL_STAT_A & (1 << UART0_DATA_EMPTY))

/* UART 1 registers */
#define UART1_CTRL_STAT_A   UCSR1A
#define UART1_CTRL_STAT_B   UCSR1B
#define UART1_CTRL_STAT_C   UCSR1C
#define UART1_BAUD_RATE_L   UBRR1L
#define UART1_BAUD_RATE_H   UBRR1H
#define UART1_DATA_REGISTER UDR1

/* Flags */
#define UART1_RX_COMPLETE   RXC1
#define UART1_DATA_EMPTY    UDRE1
#define UART1_RX_EN         RXEN1
#define UART1_TX_EN         TXEN1
#define UART1_RXC_IRQ_EN    RXCIE1
#define UART1_TXC_IRQ_EN    TXCIE1
#define UART1_8BIT_SIZE     (1 << UCSZ10) | (1 << UCSZ11)

/* UART1 helper macros */
#define UART1_RX_TX_EN      UART1_CTRL_STAT_B |= (1 << UART1_RX_EN) | (1 << UART1_TX_EN)
#define UART1_RX_IRQ_EN     UART1_CTRL_STAT_B |= (1 << UART1_RXC_IRQ_EN)
#define UART1_SET_8BIT_SIZE UART1_CTRL_STAT_C |= UART1_8BIT_SIZE
#define UART1_RECEIVED_DATA (UART1_CTRL_STAT_A & (1 << UART1_RX_COMPLETE))
#define UART1_DTREG_EMPTY   (UART1_CTRL_STAT_A & (1 << UART1_DATA_EMPTY))

/* UART 2 registers */
#define UART2_CTRL_STAT_A   UCSR2A
#define UART2_CTRL_STAT_B   UCSR2B
#define UART2_CTRL_STAT_C   UCSR2C
#define UART2_BAUD_RATE_L   UBRR2L
#define UART2_BAUD_RATE_H   UBRR2H
#define UART2_DATA_REGISTER UDR2

/* Flags */
#define UART2_RX_COMPLETE   RXC2
#define UART2_DATA_EMPTY    UDRE2
#define UART2_RX_EN         RXEN2
#define UART2_TX_EN         TXEN2
#define UART2_RXC_IRQ_EN    RXCIE2
#define UART2_TXC_IRQ_EN    TXCIE2
#define UART2_8BIT_SIZE     (1 << UCSZ20) | (1 << UCSZ21)

/* UART2 helper macros */
#define UART2_RX_TX_EN      UART2_CTRL_STAT_B |= (1 << UART2_RX_EN) | (1 << UART2_TX_EN)
#define UART2_RX_IRQ_EN     UART2_CTRL_STAT_B |= (1 << UART2_RXC_IRQ_EN)
#define UART2_SET_8BIT_SIZE UART2_CTRL_STAT_C |= UART2_8BIT_SIZE
#define UART2_RECEIVED_DATA (UART2_CTRL_STAT_A & (1 << UART2_RX_COMPLETE))
#define UART2_DTREG_EMPTY   (UART2_CTRL_STAT_A & (1 << UART2_DATA_EMPTY))

/* UART 2 registers */
#define UART3_CTRL_STAT_A   UCSR3A
#define UART3_CTRL_STAT_B   UCSR3B
#define UART3_CTRL_STAT_C   UCSR3C
#define UART3_BAUD_RATE_L   UBRR3L
#define UART3_BAUD_RATE_H   UBRR3H
#define UART3_DATA_REGISTER UDR3

/* Flags */
#define UART3_RX_COMPLETE   RXC3
#define UART3_DATA_EMPTY    UDRE3
#define UART3_RX_EN         RXEN3
#define UART3_TX_EN         TXEN3
#define UART3_RXC_IRQ_EN    RXCIE3
#define UART3_TXC_IRQ_EN    TXCIE3
#define UART3_8BIT_SIZE     (1 << UCSZ30) | (1 << UCSZ31)

/* UART3 helper macros */
#define UART3_RX_TX_EN      UART3_CTRL_STAT_B |= (1 << UART3_RX_EN) | (1 << UART3_TX_EN)
#define UART3_RX_IRQ_EN     UART3_CTRL_STAT_B |= (1 << UART3_RXC_IRQ_EN)
#define UART3_SET_8BIT_SIZE UART3_CTRL_STAT_C |= UART3_8BIT_SIZE
#define UART3_RECEIVED_DATA (UART3_CTRL_STAT_A & (1 << UART3_RX_COMPLETE))
#define UART3_DTREG_EMPTY   (UART3_CTRL_STAT_A & (1 << UART3_DATA_EMPTY))
/** @} */

/**
 * @brief SPI configuration
 *
 * The atmega2560 has only one hardware SPI with fixed pin configuration, so all
 * we can do here, is to enable or disable it...
 *
 * The fixed pins used, are:
 * MOSI - PB2 (Arduino pin 51)
 * MISO - PB3 (Arduino pin 50)
 * SCK  - PB1 (Arduino pin 52)
 * SS   - PB0 (Arduino pin 53) -> this pin is configured as output, but not used
 *
 * @{
 */
#define SPI_NUMOF           1           /* set to 0 to disable SPI */
#define SPI_0_EN            1           /* remove once SPI rework is done */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H_ */
