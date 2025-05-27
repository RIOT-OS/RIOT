/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup     boards_openmote-cc2538
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the OpenMote-cc2538 board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "cc2538_gpio.h"
#include "periph_cpu.h"

#include "cfg_clk_default.h"
#include "cfg_timer_default.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name ADC configuration
 * @{
 */
#define SOC_ADC_ADCCON3_EREF  SOC_ADC_ADCCON3_EREF_AVDD5

static const adc_conf_t adc_config[] = {
    GPIO_PIN(0, 2), /**< GPIO_PA2 = AD4_PIN */
    GPIO_PIN(0, 3), /**< GPIO_PA3 = CTS_DI07_PIN */
    GPIO_PIN(0, 4), /**< GPIO_PA4 = AD5_PIN */
    GPIO_PIN(0, 5), /**< GPIO_PA5 = AD6_PIN */
    GPIO_PIN(0, 6), /**< GPIO_PA6 = ON_SLEEP_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

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
 * @name    I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(1, 3),  /**< GPIO_PB3, OpenBattery */
        .sda_pin = GPIO_PIN(1, 4)   /**< GPIO_PB4, OpenBattery */
    },
};

#define I2C_NUMOF               ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .num      = 0,
        .mosi_pin = GPIO_PIN(0, 5),
        .miso_pin = GPIO_PIN(0, 4),
        .sck_pin  = GPIO_PIN(0, 2),
        .cs_pin   = GPIO_PIN(0, 3)
    },
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
