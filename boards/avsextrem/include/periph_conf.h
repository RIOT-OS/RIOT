/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_avsextrem
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the avsextrem
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#define XTAL_HZ             (16000000U)         /* the board provides a 16 MHz XTAL */

#define CLOCK_CORECLOCK     (72000000U)         /* this board runs with 72MHz */

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
    }
};

#define UART_NUMOF          (1)
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
        .pinsel_msk_mosi = (BIT16 | BIT17),
        .pinsel_msk_miso = (BIT14 | BIT15),
        .pinsel_msk_clk  = (BIT8  | BIT9),
    },
};

#define SPI_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
