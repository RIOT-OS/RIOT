/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nrf51dk
 * @{
 *
 * @file
 * @brief       Peripheral configuration for the Nordic nRF51DK
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "cfg_clock_16_1.h"
#include "cfg_timer_012.h"
#include "cfg_rtt_default.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    { /* Mapped to USB virtual COM port */
        .dev        = NRF_UART0,
        .rx_pin     = GPIO_PIN(0, 11),
        .tx_pin     = GPIO_PIN(0, 9),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin    = GPIO_PIN(0, 8),
        .cts_pin    = GPIO_PIN(0, 10),
#endif
        .irqn       = UART0_IRQn,
    },
};

#define UART_NUMOF          ARRAY_SIZE(uart_config)
#define UART_0_ISR          isr_uart0
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev  = NRF_SPI0,
        .sclk = 29,
        .mosi = 25,
        .miso = 28
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev = NRF_TWI1,
        .pin_scl = 7,
        .pin_sda = 30,
        .ppi     = 0,
        .speed = I2C_SPEED_NORMAL
    }
};

#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    ADC configuration
 *
 * The ADC channels have a fixed mapping:
 *
 * | Channel    | MCU Pin   | Arduino pin on board                  |
 * |:---------- |:--------- |:------------------------------------- |
 * | AIN0       | P0.26     | -- (exposed, by no Arduino UNO pin)   |
 * | AIN1       | P0.27     | -- (exposed, by no Arduino UNO pin)   |
 * | AIN2       | P0.01     | A0                                    |
 * | AIN3       | P0.02     | A1                                    |
 * | AIN4       | P0.03     | A2                                    |
 * | AIN5       | P0.04     | A3                                    |
 * | AIN6       | P0.05     | A4                                    |
 * | AIN7       | P0.06     | A5                                    |
 *
 * Expose those on Arduino pins A0 to A5
 * @{
 */
static const adc_conf_t adc_config[] = {2, 3, 4, 5, 6, 7};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
