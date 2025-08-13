/*
 * SPDX-FileCopyrightText: 2019 Beuth Hochschule für Technik Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mcb2388
 * @{
 *
 * @file
 * @brief       MCB2388 peripheral configuration
 *
 * @author      Benjamin Valentin <benpicco@beuth-hochschule.de>
 */

#include "periph_cpu.h"
#include "kernel_defines.h"

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
#define TIMER_NUMOF         (4U)
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
    {
        .dev = SPI1,
        .pinsel_mosi = 0,
        .pinsel_miso = 0,
        .pinsel_clk  = 0,
        .pinsel_msk_mosi = (BIT19), /* P0.9 */
        .pinsel_msk_miso = (BIT17), /* P0.8 */
        .pinsel_msk_clk  = (BIT15), /* P0.7 */
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
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

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev        = I2C0,
        .speed      = I2C_SPEED_NORMAL,
        .irq_prio   = 5,
        .pinsel_sda = 1,
        .pinsel_scl = 1,
        .pinsel_msk_sda = BIT22,        /* P0.27 */
        .pinsel_msk_scl = BIT24,        /* P0.28 */
    },
    {
        .dev        = I2C1,
        .speed      = I2C_SPEED_NORMAL,
        .irq_prio   = 5,
        .pinsel_sda = 1,
        .pinsel_scl = 1,
        .pinsel_msk_sda = BIT6 | BIT7,  /* P0.19 */
        .pinsel_msk_scl = BIT8 | BIT9,  /* P0.20 */
    },
};

#define I2C_NUMOF           (2)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
