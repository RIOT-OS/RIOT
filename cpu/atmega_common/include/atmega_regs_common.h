/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 INRIA
 *               2017 Thomas Perrot <thomas.perrot@tupi.fr>
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
typedef struct {
    REG8    CRA;            /**< control A */
    REG8    CRB;            /**< control B */
    REG8    CRC;            /**< control C */
    REG8    reserved;       /**< reserved */
    REG16   CNT;            /**< counter */
    REG16   ICR;            /**< input capture */
    REG16   OCR[3];         /**< output compare */
} mega_timer_t;

/**
 * @brief   8-bit timer register map
 */
typedef struct {
    REG8    CRA;            /**< control A */
    REG8    CRB;            /**< control B */
    REG8    CNT;            /**< counter */
    REG8    OCR[2];         /**< output compare */
} mini_timer_t;

/**
 * @brief   UART register map
 */
typedef struct {
    REG8    CSRA;               /**< control and status register A */
    REG8    CSRB;               /**< control and status register B */
    REG8    CSRC;               /**< control and status register C */
#ifdef CPU_ATMEGA32U4
    REG8    CSRD;               /**< control and status register D */
#else
    REG8    reserved;           /**< reserved */
#endif
    REG16   BRR;                /**< baud rate register */
    REG8    DR;                 /**< data register */
} mega_uart_t;

/**
 * @brief    Timer register definitions and instances
 * @{
 */
#if defined(TCCR0A)
#define MINI_TIMER0             ((mini_timer_t *)(uint16_t *)(&TCCR0A))
#define MINI_TIMER0_DIV         TIMER_DIV1_8_64_128_1024
#endif

#if defined(TCCR1A)
#define MEGA_TIMER1_BASE        (uint16_t *)(&TCCR1A)
#define MEGA_TIMER1             ((mega_timer_t *)MEGA_TIMER1_BASE)
#endif

#if defined(TCCR2A)
#define MINI_TIMER2             ((mini_timer_t *)(uint16_t *)(&TCCR2A))
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
