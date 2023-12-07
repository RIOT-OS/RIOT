/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_telosb
 * @{
 *
 * @file
 * @brief       TelosB peripheral configuration
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "macros/units.h"
#include "periph_cpu.h"
#include "cfg_timer_a_smclk_b_aclk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_CORECLOCK             msp430_dco_freq

/**
 * @brief   Clock configuration
 */
static const msp430_clock_params_t clock_params = {
    .target_dco_frequency = 2457600U,
    .lfxt1_frequency = 32768,
    .main_clock_source = MAIN_CLOCK_SOURCE_DCOCLK,
    .submain_clock_source = SUBMAIN_CLOCK_SOURCE_DCOCLK,
    .main_clock_divier = MAIN_CLOCK_DIVIDE_BY_1,
    .submain_clock_divier = SUBMAIN_CLOCK_DIVIDE_BY_1,
    .auxiliary_clock_divier = AUXILIARY_CLOCK_DIVIDE_BY_1,
};

/**
 * @name    UART configuration
 * @{
 */
#define UART_NUMOF          (1U)

#define UART_BASE           (&USART_1)
#define UART_SFR            (&USART_1_SFR)
#define UART_IE_RX_BIT      (1 << 4)
#define UART_IE_TX_BIT      (1 << 5)
#define UART_ME_BITS        (0x30)
#define UART_PORT           (&PORT_3)
#define UART_RX_PIN         (1 << 6)
#define UART_TX_PIN         (1 << 7)
#define UART_RX_ISR         (USART1RX_VECTOR)
#define UART_TX_ISR         (USART1TX_VECTOR)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
#define SPI_NUMOF           (1U)

#define SPI_BASE            (&USART_0)
#define SPI_SFR             (&USART_0_SFR)
#define SPI_IE_RX_BIT       (1 << 6)
#define SPI_IE_TX_BIT       (1 << 7)
#define SPI_ME_BIT          (1 << 6)
#define SPI_PIN_MISO        GPIO_PIN(P3, 2)
#define SPI_PIN_MOSI        GPIO_PIN(P3, 1)
#define SPI_PIN_CLK         GPIO_PIN(P3, 3)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
