/*
 * Copyright (C) 2019 Beuth Hochschule für Technik Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_mcb2388
 * @{
 *
 * @file
 * @brief       MCB2388 peripheral configuration
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define XTAL_HZ             (12000000U)         /* the board provides a 12 MHz XTAL */

#define CLOCK_CORECLOCK     (72000000U)         /* the lpc2388 runs with 72MHz */

#define CLOCK_PCLK          (CLOCK_CORECLOCK)
/** @} */

/**
 * @name    Timer configuration, select a number from 1 to 4
 * @{
 */
#define TIMER_NUMOF         (1U)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev = UART0,
        .irq_prio_rx = 6,
        .pinsel_rx   = 0,
        .pinsel_tx   = 0,
        .pinsel_msk_rx = BIT4,
        .pinsel_msk_tx = BIT6,
    },
    {
        .dev = UART1,
        .irq_prio_rx = 6,
        .pinsel_rx   = 1,
        .pinsel_tx   = 0,
        .pinsel_msk_rx = BIT0,
        .pinsel_msk_tx = BIT30,
    }
};

#define UART_NUMOF          (2)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev = SPI0,
        .pinsel_mosi = 3,
        .pinsel_miso = 3,
        .pinsel_clk  = 3,
        .pinsel_msk_mosi = (BIT16 | BIT17), /* P1.24 */
        .pinsel_msk_miso = (BIT14 | BIT15), /* P1.23 */
        .pinsel_msk_clk  = (BIT8  | BIT9),  /* P1.20 */
    },
};

#define SPI_NUMOF           (1)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    {
        .chan       = 0,
        .pinsel     = 1,
        .pinsel_msk = BIT14,
    },
};

#define ADC_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
