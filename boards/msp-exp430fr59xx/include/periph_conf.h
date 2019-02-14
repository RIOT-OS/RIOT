/*
 * Copyright (C) 2018 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_wolverine
 * @{
 *
 * @file
 * @brief       Wolverine peripheral configuration
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define MSP430_INITIAL_CPU_SPEED    8000000UL
#ifndef F_CPU
#define F_CPU                       MSP430_INITIAL_CPU_SPEED
#endif
#define F_RC_OSCILLATOR             32768
#define MSP430_HAS_DCOR             0
#define MSP430_HAS_EXTERNAL_CRYSTAL 1

#define CLOCK_CORECLOCK     8000000UL
#define CLOCK_CMCLK         CLOCK_CORECLOCK     /* no divider programmed */
/** @} */

/**
 * @name    DMA configuration
 * @{
 */
#define DMA_MODULE  ((msp_dma_t *)DMA_BASE)
#define DMA_CHAN    3
#define DMA_CHAN_0   ((msp_dma_channel_t *)(((char *)DMA_BASE) + 0x10))
/** @} */

/**
 * @name    Timer configuration
 * @{
 */
#define TIMER_NUMOF         1U
#define TIMER_BASE          ((msp_timer_t *)TIMER_A0_BASE)
#define TIMER_CHAN          3
#define TIMER_ISR_CC0       TIMER0_A0_VECTOR
#define TIMER_ISR_CCX       TIMER0_A1_VECTOR
#define TIMER_0_MAX_VALUE   0xffff
/** @} */

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)
#define UART_0_EN           (1U)

#define UART_USE_USCI
#define UART_BASE           ((msp_usci_t *)EUSCI_A0_BASE)
#define UART_IE             UCA0IE
#define UART_IE_RX_BIT      UCRXIE
#define UART_IE_TX_BIT      UCTXIE
#define UART_IF             UCA0IFG
#define UART_IFG_RX_BIT     UCRXIFG
#define UART_IFG_TX_BIT     UCTXIFG
#define UART_RX_ISR         USCI_A0_VECTOR
#define UART_RX_PIN         GPIO_PIN(P2, 1)
#define UART_TX_PIN         GPIO_PIN(P2, 0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
