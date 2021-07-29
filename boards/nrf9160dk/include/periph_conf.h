/*
 * Copyright (C) 2021 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    boards_nrf9160dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the nRF9160DK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = NRF_TIMER0_S,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_32Bit,
        .irqn     = TIMER0_IRQn
    },
    {
        .dev      = NRF_TIMER1_S,
        .channels = 3,
        .bitmode  = TIMER_BITMODE_BITMODE_08Bit,
        .irqn     = TIMER1_IRQn
    },
};

#define TIMER_0_ISR         isr_timer0 /**< Timer0 IRQ*/
#define TIMER_1_ISR         isr_timer1 /**< Timer1 IRQ */

#define TIMER_NUMOF         ARRAY_SIZE(timer_config) /**< Timer configuration NUMOF */
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = NRF_UARTE0_S,
        .rx_pin     = GPIO_PIN(0, 28),
        .tx_pin     = GPIO_PIN(0, 29),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE0_SPIM0_SPIS0_TWIM0_TWIS0_IRQn,
    },
    {
        .dev        = NRF_UARTE1_S,
        .rx_pin     = GPIO_PIN(0, 0),
        .tx_pin     = GPIO_PIN(0, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_UNDEF,
        .cts_pin    = GPIO_UNDEF,
#endif
        .irqn       = UARTE1_SPIM1_SPIS1_TWIM1_TWIS1_IRQn,
    },
};

#define UART_0_ISR          (isr_uarte0_spim0_spis0_twim0_twis0) /**< UART0_IRQ */
#define UART_1_ISR          (isr_uarte1_spim1_spis1_twim1_twis1) /**< UART1_IRQ */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< UART confgiguration NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
