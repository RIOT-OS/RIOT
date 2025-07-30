/*
 * SPDX-FileCopyrightText: 2014 INRIA
 * SPDX-FileCopyrightText: 2015 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
static const uart_conf_t uart_config[] = {
    {
        .uart = &usart1_as_uart,
    },
};

#define UART0_RX_ISR        (USART1RX_VECTOR)   /**< RX IRQ vector of UART 0 */
#define UART0_TX_ISR        (USART1TX_VECTOR)   /**< TX IRQ vector of UART 0 */

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .spi = &usart0_as_spi,
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
