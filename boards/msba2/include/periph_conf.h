/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_msba2
 * @{
 *
 * @file
 * @brief       MSB-A2 peripheral configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
#define XTAL_HZ             (16000000U)         /* the board provides a 16 MHz XTAL */

#define CLOCK_CORECLOCK     (72000000U)         /* the msba2 runs with 72MHz */

#define CLOCK_PCLK          (CLOCK_CORECLOCK)
/** @} */

/**
 * @name    Timer configuration, select a number from 1 to 4
 * @{
 */
#define TIMER_NUMOF         (1U)
/** @} */

/**
 * @name    PWM device and pinout configuration
 *
 * Currently, we only support a single device and 3 channels, the implementation
 * is fixed on PWM1.
 * @{
 */
#define PWM_NUMOF           (1U)

/* PWM_0 device configuration */
#define PWM_CHANNELS      (3)
#define PWM_CH0           (3)
#define PWM_CH0_MR        PWM1MR3
#define PWM_CH1           (4)
#define PWM_CH1_MR        PWM1MR4
#define PWM_CH2           (5)
#define PWM_CH2_MR        PWM1MR5
/* PWM_0 pin configuration */
#define PWM_PORT          PINSEL4
#define PWM_CH0_PIN       (2)
#define PWM_CH1_PIN       (3)
#define PWM_CH2_PIN       (4)
#define PWM_FUNC          (1)
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
        .pinsel_rx   = 4,
        .pinsel_tx   = 4,
        .pinsel_msk_rx = BIT3,
        .pinsel_msk_tx = BIT1,
    },
    {
        .dev = UART2,
        .irq_prio_rx = 6,
        .pinsel_rx   = 0,
        .pinsel_tx   = 0,
        .pinsel_msk_rx = BIT22,
        .pinsel_msk_tx = BIT20,
    },
    {
        .dev = UART3,
        .irq_prio_rx = 6,
        .pinsel_rx   = 9,
        .pinsel_tx   = 9,
        .pinsel_msk_rx = BIT26 | BIT27,
        .pinsel_msk_tx = BIT24 | BIT25,
    },
};

#define UART_NUMOF          (4)
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

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    {   /* P0.23 */
        .chan       = 0,
        .pinsel     = 1,
        .pinsel_msk = BIT14,
    },
    {   /* P0.24 */
        .chan       = 1,
        .pinsel     = 1,
        .pinsel_msk = BIT16,
    },
    {   /* P0.25 */
        .chan       = 2,
        .pinsel     = 1,
        .pinsel_msk = BIT18,
    },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

/**
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {   /* JP3 */
        .dev        = I2C2,
        .speed      = I2C_SPEED_NORMAL,
        .irq_prio   = 5,
        .pinsel_sda = 0,
        .pinsel_scl = 0,
        .pinsel_msk_sda = BIT21, /* P0.10 */
        .pinsel_msk_scl = BIT23, /* P0.11 */
    },
};

/* used in arithmetic preprocessor expression, so no ARRAY_SIZE() */
#define I2C_NUMOF           (1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
