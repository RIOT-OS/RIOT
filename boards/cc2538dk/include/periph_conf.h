/*
 * SPDX-FileCopyrightText: 2014 Loci Controls Inc.
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_cc2538dk
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the CC2538DK board
 *
 * @author      Ian Martin <ian@locicontrols.com>
 */

#include "cpu.h"
#include "periph_cpu.h"

#include "cfg_clk_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev      = UART0_BASEADDR,
        .rx_pin   = GPIO_PIN(0, 0),
        .tx_pin   = GPIO_PIN(0, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_uart0

/* macros common across all UARTs */
#define UART_NUMOF          ARRAY_SIZE(uart_config)

/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(0, 2),  /**< GPIO_PA2, SPI_SCK  on SmartRF06 */
        .sda_pin = GPIO_PIN(0, 4)   /**< GPIO_PA4, SPI_MOSI on SmartRF06 */
    },
};

#define I2C_NUMOF               ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(0, 4),
        .miso_pin = GPIO_PIN(0, 5),
        .sck_pin  = GPIO_PIN(0, 2),
        .cs_pin   = GPIO_PIN(3, 0)
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON3_EREF  SOC_ADC_ADCCON3_EREF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(0, 6), /**< GPIO_PA6 = ADC_ALS_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
