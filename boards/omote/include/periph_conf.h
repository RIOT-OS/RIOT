/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2017 HAW Hamburg
 * SPDX-FileCopyrightText: 2020 Oppila Microsystems -  http://www.oppila.in
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_omote
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the omote board
 *
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
    GPIO_PIN(PORT_A, 5), /**< GPIO_PA5 = ADC1_PIN,LDR sensor is connected */
    GPIO_PIN(PORT_A, 4), /**< GPIO_PA4 = ADC2_PIN */
    GPIO_PIN(PORT_A, 2), /**< GPIO_PA2 = ADC3_PIN */
    GPIO_PIN(PORT_A, 6), /**< GPIO_PA6 = ADC4_PIN */
    GPIO_PIN(PORT_A, 7), /**< GPIO_PA7 = ADC5_PIN */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */
/**
 * @name I2C configuration
 * @{
 */
#define I2C_IRQ_PRIO            1

static const i2c_conf_t i2c_config[] = {
    {
        .speed = I2C_SPEED_FAST,    /**< bus speed */
        .scl_pin = GPIO_PIN(PORT_C, 3),  /**< GPIO_PC3 */
        .sda_pin = GPIO_PIN(PORT_C, 2)   /**< GPIO_PC2 */
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
        .mosi_pin = GPIO_PIN(PORT_C, 5),
        .miso_pin = GPIO_PIN(PORT_C, 6),
        .sck_pin  = GPIO_PIN(PORT_C, 4),
        .cs_pin   = GPIO_PIN(PORT_A, 7)
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */
/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    /* UART0 is mapped to debug usb */
    {
        .dev      = UART0_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_A, 0),
        .tx_pin   = GPIO_PIN(PORT_A, 1),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    },
    {
        .dev      = UART1_BASEADDR,
        .rx_pin   = GPIO_PIN(PORT_C, 1),
        .tx_pin   = GPIO_PIN(PORT_C, 0),
#ifdef MODULE_PERIPH_UART_HW_FC
        .cts_pin  = GPIO_UNDEF,
        .rts_pin  = GPIO_UNDEF
#endif
    }
};
/* interrupt function name mapping */
#define UART_0_ISR          isr_uart0
#define UART_1_ISR          isr_uart1

/* macros common across all UARTs */
#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
