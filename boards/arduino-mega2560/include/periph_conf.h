/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2014 Hinnerk van Bruinehsen
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
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
 * @brief   Timer peripheral configuration
 *
 * The ATmega2560 has 6 timers. Timer0 and Timer2 are 8 Bit Timers,
 * Timer0 has special uses too and therefore we'll avoid using it.
 * Timer5 has special uses with certain Arduino Shields, too.
 * Therefore we'll also avoid using Timer5.
 * This results in the following mapping to use the left over 16 Bit
 * timers:
 *
 * Timer1 -> TIMER_0
 * Timer3 -> TIMER_1
 * Timer4 -> TIMER_2
 *
 * @{
 */
#define TIMER_NUMOF         (3U)
#define TIMER_0_EN          1
#define TIMER_1_EN          0
#define TIMER_2_EN          0

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    3
#define TIMER_0_MAX_VALUE   (0xffff)

/* Timer 0 register and flags */
#define TIMER0_CONTROL_A    TCCR1A
#define TIMER0_CONTROL_B    TCCR1B
#define TIMER0_CONTROL_C    TCCR1C
#define TIMER0_COUNTER      TCNT1
#define TIMER0_IRQ_FLAG_REG TIFR1
#define TIMER0_IRQ_MASK_REG TIMSK1
#define TIMER0_COMP_A       OCR1A
#define TIMER0_COMP_B       OCR1B
#define TIMER0_COMP_C       OCR1C
#define TIMER0_COMP_A_FLAG  OCF1A
#define TIMER0_COMP_B_FLAG  OCF1B
#define TIMER0_COMP_C_FLAG  OCF1C
#define TIMER0_COMP_A_EN    OCIE1A
#define TIMER0_COMP_B_EN    OCIE1B
#define TIMER0_COMP_C_EN    OCIE1C
#define TIMER0_FREQ_16MHZ   (0 << CS12) | (0 << CS11) | (1 << CS10)
#define TIMER0_FREQ_2MHZ    (0 << CS12) | (1 << CS11) | (0 << CS10)
#define TIMER0_FREQ_250KHZ  (0 << CS12) | (1 << CS11) | (1 << CS10)
#define TIMER0_FREQ_DISABLE (0 << CS12) | (0 << CS11) | (0 << CS10)
#define TIMER0_COMPA_ISR    TIMER1_COMPA_vect
#define TIMER0_COMPB_ISR    TIMER1_COMPB_vect
#define TIMER0_COMPC_ISR    TIMER1_COMPC_vect

/* Timer 1 configuration */
#define TIMER_1_CHANNELS    3
#define TIMER_1_MAX_VALUE   (0xffff)

/* Timer 1 register and flags */
#define TIMER1_CONTROL_A    TCCR3A
#define TIMER1_CONTROL_B    TCCR3B
#define TIMER1_CONTROL_C    TCCR3C
#define TIMER1_COUNTER      TCNT3
#define TIMER1_IRQ_FLAG_REG TIFR3
#define TIMER1_IRQ_MASK_REG TIMSK3
#define TIMER1_COMP_A       OCR3A
#define TIMER1_COMP_B       OCR3B
#define TIMER1_COMP_C       OCR3C
#define TIMER1_COMP_A_FLAG  OCF3A
#define TIMER1_COMP_B_FLAG  OCF3B
#define TIMER1_COMP_C_FLAG  OCF3C
#define TIMER1_COMP_A_EN    OCIE3A
#define TIMER1_COMP_B_EN    OCIE3B
#define TIMER1_COMP_C_EN    OCIE3C
#define TIMER1_FREQ_16MHZ   (0 << CS32) | (0 << CS31) | (1 << CS30)
#define TIMER1_FREQ_2MHZ    (0 << CS32) | (1 << CS31) | (0 << CS30)
#define TIMER1_FREQ_250KHZ  (0 << CS32) | (1 << CS31) | (1 << CS30)
#define TIMER1_FREQ_DISABLE (0 << CS32) | (0 << CS31) | (0 << CS30)
#define TIMER1_COMPA_ISR    TIMER3_COMPA_vect
#define TIMER1_COMPB_ISR    TIMER3_COMPB_vect
#define TIMER1_COMPC_ISR    TIMER3_COMPC_vect

/* Timer 2 configuration */
#define TIMER_2_CHANNELS    3
#define TIMER_2_MAX_VALUE   (0xffff)

/* Timer 2 register and flags */
#define TIMER2_CONTROL_A    TCCR4A
#define TIMER2_CONTROL_B    TCCR4B
#define TIMER2_CONTROL_C    TCCR4C
#define TIMER2_COUNTER      TCNT4
#define TIMER2_IRQ_FLAG_REG TIFR4
#define TIMER2_IRQ_MASK_REG TIMSK4
#define TIMER2_COMP_A       OCR4A
#define TIMER2_COMP_B       OCR4B
#define TIMER2_COMP_C       OCR4C
#define TIMER2_COMP_A_FLAG  OCF4A
#define TIMER2_COMP_B_FLAG  OCF4B
#define TIMER2_COMP_C_FLAG  OCF4C
#define TIMER2_COMP_A_EN    OCIE4A
#define TIMER2_COMP_B_EN    OCIE4B
#define TIMER2_COMP_C_EN    OCIE4C
#define TIMER2_FREQ_16MHZ   (0 << CS42) | (0 << CS41) | (1 << CS40)
#define TIMER2_FREQ_2MHZ    (0 << CS42) | (1 << CS41) | (0 << CS40)
#define TIMER2_FREQ_250KHZ  (0 << CS42) | (1 << CS41) | (1 << CS40)
#define TIMER2_FREQ_DISABLE (0 << CS42) | (0 << CS41) | (0 << CS40)
#define TIMER2_COMPA_ISR    TIMER4_COMPA_vect
#define TIMER2_COMPB_ISR    TIMER4_COMPB_vect
#define TIMER2_COMPC_ISR    TIMER4_COMPC_vect
/** @} */

/**
 * @brief   UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#define UART_NUMOF          (4U)

#define UART_0              MEGA_UART0
#define UART_0_ISR          USART0_RX_vect

#define UART_1              MEGA_UART1
#define UART_1_ISR          USART1_RX_vect

#define UART_2              MEGA_UART2
#define UART_2_ISR          USART2_RX_vect

#define UART_3              MEGA_UART3
#define UART_3_ISR          USART3_RX_vect
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
