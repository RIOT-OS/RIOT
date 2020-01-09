/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_esp32
 * @brief       Common declarations of ESP32 periphery for all ESP32 boards
 *
 * This file contains peripheral configurations that are valid for all ESP32.
 *
 * For detailed information about the configuration of ESP32 boards, see
 * section \ref esp32_comm_periph "Common Peripherals".
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 * @{
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

/* include periph_cpu.h to make it visible in any case */
#include "periph_cpu.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    ADC configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as ADC channels
 *
 * ADC_GPIOS is defined in board-specific peripheral configuration. Since
 * ADC_GPIOS must be defined even if there are no ADC channels, an empty
 * list definition is done here as fallback configuration.
 */
#ifndef ADC_GPIOS
#define ADC_GPIOS   { }
#endif

/**
 * @brief   Static array with declared ADC channels
 */
static const gpio_t adc_channels[] = ADC_GPIOS;

/**
 * @brief Number of GPIOs declared as ADC channels
 *
 * The number of GPIOs that are declared as ADC channels is determined from
 * the ADC_GPIOS definition.
 *
 * @note ADC_NUMOF definition must not be changed.
 */
#define ADC_NUMOF   ARRAY_SIZE(adc_channels)
/** @} */

/**
 * @name    DAC configuration
 * @{
 */

/**
 * @brief   Declaration of GPIOs that can be used as DAC channels
 *
 * DAC_GPIOS is defined in board-specific peripheral configuration. Since
 * DAC_GPIOS must be defined even if there are no DAC channels, an empty
 * list definition is done here as fallback configuration.
 */
#ifndef DAC_GPIOS
#define DAC_GPIOS   { }
#endif

/**
 * @brief   Static array with declared DAC channels
 */
static const gpio_t dac_channels[] = DAC_GPIOS;

/**
 * @brief Number of GPIOs declared as DAC channels
 *
 * The number of GPIOs that are declared as DAC channels is determined from
 * the DAC_GPIOS definition.
 *
 * @note DAC_NUMOF definition must not be changed.
 */
#define DAC_NUMOF   ARRAY_SIZE(dac_channels)
/** @} */

/**
 * @name   I2C configuration
 * @{
 */

/**
 * @brief   Static array with configuration for declared I2C devices
 */
static const i2c_conf_t i2c_config[] = {
    #if defined(I2C0_SCL) && defined(I2C0_SDA) && defined(I2C0_SPEED)
    {
        .speed = I2C0_SPEED,
        .scl = I2C0_SCL,
        .sda = I2C0_SDA,
    },
    #endif
    #if defined(I2C1_SCL) && defined(I2C1_SDA) && defined(I2C1_SPEED)
    {
        .speed = I2C1_SPEED,
        .scl = I2C1_SCL,
        .sda = I2C1_SDA,
    },
    #endif
};

/**
 * @brief Number of I2C interfaces
 *
 * The number of I2C interfaces is determined from board-specific peripheral
 * definitions of I2Cn_SPEED, I2Cn_SCK, and I2Cn_SDA.
 *
 * @note I2C_NUMOF definition must not be changed.
 */
#define I2C_NUMOF   ARRAY_SIZE(i2c_config)

/** @} */

/**
 * @name   PWM configuration
 * @{
 */

/**
 * @brief   Static array of GPIOs that can be used as channels of PWM0
 */
#ifdef PWM0_GPIOS
static const gpio_t pwm0_channels[] = PWM0_GPIOS;
#endif
/**
 * @brief   Static array of GPIOs that can be used as channels of PWM0
 */
#ifdef PWM1_GPIOS
static const gpio_t pwm1_channels[] = PWM1_GPIOS;
#endif

/**
 * @brief   Number of PWM devices
 *
 * The number of PWM devices is determined from the PWM0_GPIOS and PWM1_GPIOS
 * definitions.
 *
 * @note PWM_NUMOF definition must not be changed.
 */
#if defined(PWM0_GPIOS) && defined(PWM1_GPIOS)
#define PWM_NUMOF  (2)
#elif defined(PWM0_GPIOS) || defined(PWM1_GPIOS)
#define PWM_NUMOF  (1)
#else
#define PWM_NUMOF  (0)
#endif

/** @} */

/**
 * @name   SPI configuration
 */

/**
 * @brief   Static array with configuration for declared SPI devices
 */
static const spi_conf_t spi_config[] = {
#ifdef SPI0_CTRL
    {
        .ctrl = SPI0_CTRL,
        .sck = SPI0_SCK,
        .mosi = SPI0_MOSI,
        .miso = SPI0_MISO,
        .cs = SPI0_CS0,
    },
#endif
#ifdef SPI1_CTRL
    {
        .ctrl = SPI1_CTRL,
        .sck = SPI1_SCK,
        .mosi = SPI1_MOSI,
        .miso = SPI1_MISO,
        .cs = SPI1_CS0,
    },
#endif
};

/**
 * @brief Number of SPI interfaces
 *
 * The number of SPI interfaces is determined from board-specific peripheral
 * definitions of SPIn_*.
 *
 * @note SPI_NUMOF definition must not be changed.
 */
#define SPI_NUMOF   ARRAY_SIZE(spi_config)

/** @} */

/**
 * @name   UART configuration
 */

#ifndef UART0_TXD
#define UART0_TXD   (GPIO1)  /**< TxD of UART_DEV(0) used on all ESP32 boards */
#endif
#ifndef UART0_RXD
#define UART0_RXD   (GPIO3)  /**< RxD of UART_DEV(0) used on all ESP32 boards */
#endif

/**
 * @brief   Static array with configuration for declared UART devices
 */
static const uart_conf_t uart_config[] = {
    {
        .txd = UART0_TXD,
        .rxd = UART0_RXD,
    },
    #if defined(UART1_TXD) && defined(UART1_RXD)
    {
        .txd = UART1_TXD,
        .rxd = UART1_RXD,
    },
    #endif
    #if defined(UART2_TXD) && defined(UART2_RXD)
    {
        .txd = UART2_TXD,
        .rxd = UART2_RXD,
    },
    #endif
};

/**
 * @brief Number of UART interfaces
 *
 * The number of UART interfaces is determined from board-specific peripheral
 * definitions of UARTn_*.
 *
 * @note UART_NUMOF definition must not be changed.
 */
#define UART_NUMOF  ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
