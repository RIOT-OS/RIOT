/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2016 INRIA
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
 */

#ifndef ATMEGA_REGS_COMMON_H
#define ATMEGA_REGS_COMMON_H

#include <avr/io.h>

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
 * @brief   UART register map
 */
typedef struct {
    REG8    CSRA;               /**< control and status register A */
    REG8    CSRB;               /**< control and status register B */
    REG8    CSRC;               /**< control and status register C */
    REG8    reserved;           /**< reserved */
    REG16   BRR;                /**< baud rate register */
    REG8    DR;                 /**< data register */
} mega_uart_t;

/**
 * @brief    Base register address definitions
 * @{
 */
#define MEGA_TIMER1_BASE        (uint16_t *)(&TCCR1A)
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1281__)
#define MEGA_TIMER3_BASE        (uint16_t *)(&TCCR3A)
#define MEGA_TIMER4_BASE        (uint16_t *)(&TCCR4A)
#define MEGA_TIMER5_BASE        (uint16_t *)(&TCCR5A)
#endif
/** @} */

/**
 * @brief    Base register address definitions
 * @{
 */
#define MEGA_UART0_BASE         ((uint16_t *)(&UCSR0A))
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1281__)
#define MEGA_UART1_BASE         ((uint16_t *)(&UCSR1A))
#define MEGA_UART2_BASE         ((uint16_t *)(&UCSR2A))
#define MEGA_UART3_BASE         ((uint16_t *)(&UCSR3A))
#endif
/** @} */

/**
 * @brief    Peripheral instances
 * @{
 */
#define MEGA_TIMER1             ((mega_timer_t *)MEGA_TIMER1_BASE)
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1281__)
#define MEGA_TIMER3             ((mega_timer_t *)MEGA_TIMER3_BASE)
#define MEGA_TIMER4             ((mega_timer_t *)MEGA_TIMER4_BASE)
#define MEGA_TIMER5             ((mega_timer_t *)MEGA_TIMER5_BASE)
#endif
/** @} */

/**
 * @brief    Peripheral instances
 * @{
 */
#define MEGA_UART0              ((mega_uart_t *)MEGA_UART0_BASE)
#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1281__)
#define MEGA_UART1              ((mega_uart_t *)MEGA_UART1_BASE)
#define MEGA_UART2              ((mega_uart_t *)MEGA_UART2_BASE)
#define MEGA_UART3              ((mega_uart_t *)MEGA_UART3_BASE)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ATMEGA2560_REGS_H */
/** @} */
