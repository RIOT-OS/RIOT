/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_z1
 * @{
 *
 * @file
 * @brief       Zolertia Z1 peripheral configuration
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
#define CLOCK_CORECLOCK     (8000000U)

#define CLOCK_CMCLK         CLOCK_CORECLOCK     /* no divider programmed */
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_NUMOF         (1U)
#define TIMER_BASE          (TIMER_A)
#define TIMER_CHAN          (3)
#define TIMER_ISR_CC0       (TIMERA0_VECTOR)
#define TIMER_ISR_CCX       (TIMERA1_VECTOR)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           (1U)

#define UART_USE_USCI
#define UART_BASE           (USCI_0)
#define UART_IE             (SFR->IE2)
#define UART_IF             (SFR->IFG2)
#define UART_IE_RX_BIT      (1 << 0)
#define UART_IE_TX_BIT      (1 << 1)
#define UART_RX_PORT        ((msp_port_t *)PORT_3)
#define UART_RX_PIN         (1 << 5)
#define UART_TX_PORT        ((msp_port_t *)PORT_3)
#define UART_TX_PIN         (1 << 4)
#define UART_RX_ISR         (USCIAB0RX_VECTOR)
#define UART_TX_ISR         (USCIAB0TX_VECTOR)
/** @} */

 /**
 * @name   SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)

/* SPI configuration */
#define SPI_USE_USCI
#define SPI_BASE            (USCI_0_B_SPI)
#define SPI_IE              (SFR->IE2)
#define SPI_IF              (SFR->IFG2)
#define SPI_IE_RX_BIT       (1 << 2)
#define SPI_IE_TX_BIT       (1 << 3)
#define SPI_PIN_MISO        GPIO_PIN(P3,2)
#define SPI_PIN_MOSI        GPIO_PIN(P3,1)
#define SPI_PIN_CLK         GPIO_PIN(P3,3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
