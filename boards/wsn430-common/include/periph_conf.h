/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_wsn430-common
 * @{
 *
 * @file
 * @brief       WSN30 peripheral configuration
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *              Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define CLOCK_CORECLOCK     (8000000U)

#define CLOCK_CMCLK         CLOCK_CORECLOCK     /* no divider programmed */
/** @} */

/**
 * @brief   Timer configuration
 * @{
 */
#define TIMER_DEV           (TIMER_A)
#define TIMER_CHAN          (3)
#define TIMER_ISR_CC0       (TIMERA0_VECTOR)
#define TIMER_ISR_CCX       (TIMERA1_VECTOR)
/** @} */

/**
 * @brief   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           (1U)

#define UART_DEV            (USART_0)
#define UART_IE             (SFR->IE1)
#define UART_IF             (SFR->IFG1)
#define UART_IE_RX_BIT      (1 << 6)
#define UART_IE_TX_BIT      (1 << 7)
#define UART_ME             (SFR->ME1)
#define UART_ME_BITS        (0xc0)
#define UART_PORT           (PORT_3)
#define UART_RX_PIN         (1 << 4)
#define UART_TX_PIN         (1 << 5)
#define UART_RX_ISR         (USART0RX_VECTOR)
#define UART_TX_ISR         (USART0TX_VECTOR)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
