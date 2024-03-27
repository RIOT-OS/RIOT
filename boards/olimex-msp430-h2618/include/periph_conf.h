/*
 * Copyright (C) 2014 INRIA
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_olimex_msp430_h2618
 * @{
 *
 * @file
 * @brief       Olimex-MSP430-H2618 peripheral configuration
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "macros/units.h"
#include "cfg_timer_a_smclk_b_aclk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_CORECLOCK     msp430_dco_freq

/**
 * @brief   Clock configuration
 */
static const msp430_clock_params_t clock_params = {
    .target_dco_frequency = MHZ(16),
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
        .uart = &usci_a1_as_uart,
    },
};

#define UART0_RX_ISR        USCIAB1RX_VECTOR    /**< RX IRQ vector of UART 0 */

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .spi = &usci_b0_as_spi,
    },
    {
        .spi = &usci_b1_as_spi,
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
