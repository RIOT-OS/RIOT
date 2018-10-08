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
 * @brief Number of GPIOs declared as ADC channels
 *
 * The number of GPIOs that are declared as ADC channels is determined from
 * the ADC_GPIOS definition.
 *
 * @note ADC_NUMOF definition must not be changed.
 */
#define ADC_NUMOF   (adc_chn_num)
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
 * @brief Number of GPIOs declared as DAC channels
 *
 * The number of GPIOs that are declared as DAC channels is determined from
 * the DAC_GPIOS definition.
 *
 * @note DAC_NUMOF definition must not be changed.
 */
#define DAC_NUMOF   (dac_chn_num)
/** @} */


/**
 * @name   I2C configuration
 * @{
 */

/**
 * @brief Number of I2C interfaces
 *
 * The number of I2C interfaces is determined from board-specific peripheral
 * definitions of I2Cn_SPEED, I2Cn_SCK, and I2Cn_SDA.
 *
 * @note I2C_NUMOF definition must not be changed.
 */
#define I2C_NUMOF   (i2c_bus_num)

/** @} */

/**
 * @name   PWM configuration
 * @{
 */

/**
 * @brief   Number of PWM devices
 *
 * The number of PWM devices is determined from the PWM0_GPIOS and PWM1_GPIOS
 * definitions.
 *
 * @note PWM_NUMOF definition must not be changed.
 */
#define PWM_NUMOF   (pwm_dev_num)

/** @} */

/**
 * @name   SPI configuration
 */

/**
 * @brief Number of SPI interfaces
 *
 * The number of SPI interfaces is determined from board-specific peripheral
 * definitions of SPIn_*.
 *
 * @note SPI_NUMOF definition must not be changed.
 */
#define SPI_NUMOF   (spi_bus_num)

/** @} */

/**
 * @name   UART configuration
 */

/**
 * @brief Number of UART interfaces
 *
 * The number of UART interfaces is determined from board-specific peripheral
 * definitions of UARTn_*.
 *
 * @note UART_NUMOF definition must not be changed.
 */
#if defined(UART1_TXD) && defined(UART1_RXD) && defined(UART2_TXD) && defined(UART2_RXD)
#define UART_NUMOF  3
#elif (defined(UART1_TXD) && defined(UART1_RXD)) || (defined(UART2_TXD) && defined(UART2_RXD))
#define UART_NUMOF  2
#else
#define UART_NUMOF  1
#endif
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
