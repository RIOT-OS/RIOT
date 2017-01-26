/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_chronos
 * @{
 *
 * @file
 * @brief       Chronos peripheral configuration
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Clock configuration
 *
 * @todo    Move all clock configuration code here from the board.h
 */
#define CLOCK_CORECLOCK     (12000000U)

#define CLOCK_CMCLK         CLOCK_CORECLOCK     /* no divider programmed */
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_BASE          (TIMER_A0)
#define TIMER_CHAN          (5)
#define TIMER_ISR_CC0       (TIMER0_A0_VECTOR)
#define TIMER_ISR_CCX       (TIMER0_A1_VECTOR)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           (1U)

#define UART_DEV            (USART_1)
#define UART_IE             (SFR->IE2)
#define UART_IF             (SFR->IFG2)
#define UART_IE_RX_BIT      (1 << 4)
#define UART_IE_TX_BIT      (1 << 5)
#define UART_ME             (SFR->ME2)
#define UART_ME_BITS        (0x30)
#define UART_PORT           (PORT_3)
#define UART_RX_PIN         (1 << 6)
#define UART_TX_PIN         (1 << 7)
#define UART_RX_ISR         (USART1RX_VECTOR)
#define UART_TX_ISR         (USART1TX_VECTOR)
/** @} */

/**
 * @brief   Real Time Clock configuration
 */
#define RTC_NUMOF           (1)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
