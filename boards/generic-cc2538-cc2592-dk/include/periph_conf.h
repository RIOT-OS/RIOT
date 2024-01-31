/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_generic_cc2538_cc2592_dk
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Generic CC2538-CC2592-DK
 *              board
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

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
        .rx_pin   = GPIO_PIN(PORT_A, 0),
        .tx_pin   = GPIO_PIN(PORT_A, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    }
};

#define UART_0_ISR          isr_uart0   /**< interrupt function name mapping */

#define UART_NUMOF          ARRAY_SIZE(uart_config) /**< number of UARTs */

/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(PORT_A, 2),  /**< GPIO_PA2, SPI_SCK  on SmartRF06 */
        .sda_pin = GPIO_PIN(PORT_A, 4)   /**< GPIO_PA4, SPI_MOSI on SmartRF06 */
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
        .mosi_pin = GPIO_PIN(PORT_A, 4),
        .miso_pin = GPIO_PIN(PORT_A, 5),
        .sck_pin  = GPIO_PIN(PORT_A, 2),
        .cs_pin   = GPIO_PIN(PORT_D, 0)
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
    GPIO_PIN(PORT_A, 6), /**< GPIO_PA6 = ADC_ALS_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_H */
/** @} */
