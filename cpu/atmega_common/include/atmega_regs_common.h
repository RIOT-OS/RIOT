/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 INRIA
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
 *               2023 Hugues Larrive
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file
 * @brief       CMSIS style register definitions for the atmega family
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Thomas Perrot <thomas.perrot@tupi.fr>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 */

#ifndef ATMEGA_REGS_COMMON_H
#define ATMEGA_REGS_COMMON_H

#include <avr/io.h>
#include <avr/power.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Register types
 * @{
 */
#define REG8                    volatile uint8_t
#define REG16                   volatile uint16_t
/** @} */

/**
 * @brief   Timer register map
 */
#ifdef TCCR1C
typedef struct {
    REG8    CRA;            /**< control A */
    REG8    CRB;            /**< control B */
    REG8    CRC;            /**< control C */
    REG8    reserved;       /**< reserved */
    REG16   CNT;            /**< counter */
    REG16   ICR;            /**< input capture */
    REG16   OCR[3];         /**< output compare */
} mega_timer_t;
#else /* atmega8 */
typedef struct {
    REG16   ICR;            /**< input capture */
    REG16   OCR[2];         /**< output compare */
    REG16   CNT;            /**< counter */
    REG8    CRB;            /**< control B */
    REG8    CRA;            /**< control A */
} mega_timer_t;
#endif

/**
 * @brief   8-bit timer register map
 */
typedef struct {
#if ((defined(TCCR0A) && defined(TCCR0B)) || (defined(TCCR2A) && defined(TCCR2B)))
    REG8    CRA;            /**< control A */
    REG8    CRB;            /**< control B */
    REG8    CNT;            /**< counter */
    REG8    OCR[2];         /**< output compare */
#elif defined(TCCR2)
    REG8    OCR;            /**< output compare */
    REG8    CNT;            /**< counter */
    REG8    CR;             /**< control */
#endif
} mini_timer_t;

/**
 * @brief   UART register map
 */
typedef struct {
#if defined(UCSR0A) || defined(UCSR1A)
    REG8    CSRA;               /**< control and status register A */
    REG8    CSRB;               /**< control and status register B */
    REG8    CSRC;               /**< control and status register C */
#ifdef UCSR1D /* 32u4 */
    REG8    CSRD;               /**< control and status register D */
#else
    REG8    reserved;           /**< reserved */
#endif
    REG16   BRR;                /**< baud rate register */
    REG8    DR;                 /**< data register */
#elif defined(UCSRA) /* atmega8 */
    REG8    BRRL;               /**< baud rate register low byte */
    REG8    CSRB;               /**< control and status register B */
    REG8    CSRA;               /**< control and status register A */
    REG8    DR;                 /**< data register */
    REG8    padding[19];        /**< 3 SPI + 3 PORTD + 3 PORTC + 3 PORTB
                                 *   + 3 reserved + 4 EEPROM = 19 */
    REG8    CSRC;               /**< control and status register C shared
                                 *   with baud rate register high byte */
#endif
} mega_uart_t;

/**
 * @brief    Timer register definitions and instances
 * @{
 */
#if defined(TCCR0A)
#define MINI_TIMER0             ((mini_timer_t *)(uint16_t *)(&TCCR0A))
#define MINI_TIMER0_DIV         TIMER_DIV1_8_64_128_1024
#endif

#if defined(TCCR1C)
#define MEGA_TIMER1_BASE        (uint16_t *)(&TCCR1A)
#define MEGA_TIMER1             ((mega_timer_t *)MEGA_TIMER1_BASE)
#elif defined(TCCR1A) /* atmega8 */
#define MEGA_TIMER1_BASE        (uint16_t *)(&ICR1L)
#define MEGA_TIMER1             ((mega_timer_t *)MEGA_TIMER1_BASE)
#endif

#if defined(TCCR2A)
#define MINI_TIMER2             ((mini_timer_t *)(uint16_t *)(&TCCR2A))
#define MINI_TIMER2_DIV         TIMER_DIV1_8_32_64_128_256_1024
#elif defined(TCCR2) /* atmega8 */
#define MINI_TIMER2             ((mini_timer_t *)(uint16_t *)(&OCR2))
#define MINI_TIMER2_DIV         TIMER_DIV1_8_32_64_128_256_1024
#endif

#if defined(TCCR3A)
#define MEGA_TIMER3_BASE        (uint16_t *)(&TCCR3A)
#define MEGA_TIMER3             ((mega_timer_t *)MEGA_TIMER3_BASE)
#endif

#if defined(TCCR4A)
#define MEGA_TIMER4_BASE        (uint16_t *)(&TCCR4A)
#define MEGA_TIMER4             ((mega_timer_t *)MEGA_TIMER4_BASE)
#endif

#if defined(TCCR5A)
#define MEGA_TIMER5_BASE        (uint16_t *)(&TCCR5A)
#define MEGA_TIMER5             ((mega_timer_t *)MEGA_TIMER5_BASE)
#endif
/** @} */

/**
 * @brief    Peripheral register definitions and instances
 * @{
 */
#if defined(UCSRA)
#define MEGA_UART_BASE         ((uint16_t *)(&UBRRL))
#define MEGA_UART              ((mega_uart_t *)MEGA_UART_BASE)
#endif

#if defined(UCSR0A)
#define MEGA_UART0_BASE         ((uint16_t *)(&UCSR0A))
#define MEGA_UART0              ((mega_uart_t *)MEGA_UART0_BASE)
#endif

#if defined(UCSR1A)
#define MEGA_UART1_BASE         ((uint16_t *)(&UCSR1A))
#define MEGA_UART1              ((mega_uart_t *)MEGA_UART1_BASE)
#endif

#if defined(UCSR2A)
#define MEGA_UART2_BASE         ((uint16_t *)(&UCSR2A))
#define MEGA_UART2              ((mega_uart_t *)MEGA_UART2_BASE)
#endif

#if defined(UCSR3A)
#define MEGA_UART3_BASE         ((uint16_t *)(&UCSR3A))
#define MEGA_UART3              ((mega_uart_t *)MEGA_UART3_BASE)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ATMEGA_REGS_COMMON_H */
/** @} */
