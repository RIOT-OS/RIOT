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

#ifndef __PERIPH_CONF_H
#define __PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name        Timer peripheral configuration
 * @brief       The ATmega2560 has 6 timers. Timer0 and Timer2 are 8 Bit Timers,
 *              Timer0 has special uses too and therefore we'll avoid using it.
 *              Timer5 has special uses with certain Arduino Shields, too.
 *              Therefore we'll also avoid using Timer5.
 *              This results in the following mapping to use the left over 16 Bit
 *              timers:
 *
 *              Timer1 -> TIMER_0
 *              Timer3 -> TIMER_1
 *              Timer4 -> TIMER_2
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
#define TIMER2_FREQ_DISABLE (0 << CS42) | (0 << CS41) | (0 << CS40)
#define TIMER2_COMPA_ISR    TIMER4_COMPA_vect
#define TIMER2_COMPB_ISR    TIMER4_COMPB_vect
#define TIMER2_COMPC_ISR    TIMER4_COMPC_vect

/**
 * @name UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           1
#define UART_1_EN           0
#define UART_2_EN           0
#define UART_3_EN           0

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



/* TODO: add defines for device agnostic implementation */
/** @} */


/**
 * @brief ADC configuration
 */
#define ADC_NUMOF           (0U)
#define ADC_0_EN            0
#define ADC_1_EN            0

/**
 * @brief PWM configuration
 */
#define PWM_NUMOF           (0U)
#define PWM_0_EN            0
#define PWM_1_EN            0

/**
 * @brief SPI configuration
 */
#define SPI_NUMOF           (0U)                                                    /* TODO */
#define SPI_0_EN            0
#define SPI_1_EN            0

/**
 * @brief I2C configuration
 */
#define I2C_NUMOF           (0U)                                                    /* TODO */
#define I2C_0_EN            0
#define I2C_0_EN            0

/**
 * @brief GPIO configuration
 */
#define GPIO_NUMOF          (0U)
#define GPIO_0_EN           0
#define GPIO_1_EN           0
#define GPIO_2_EN           0
#define GPIO_3_EN           0
#define GPIO_4_EN           0
#define GPIO_5_EN           0
#define GPIO_6_EN           0
#define GPIO_7_EN           0
#define GPIO_8_EN           0
#define GPIO_9_EN           0
#define GPIO_10_EN          0
#define GPIO_11_EN          0
#define GPIO_12_EN          0
#define GPIO_13_EN          0
#define GPIO_14_EN          0
#define GPIO_15_EN          0

#ifdef __cplusplus
}
#endif

#endif /* __PERIPH_CONF_H */
