/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_atxmega
 * @{
 *
 * @file
 * @brief       Common configuration of MCU periphery for ATxmega boards
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef PERIPH_CONF_COMMON_H
#define PERIPH_CONF_COMMON_H

#include "periph_cpu.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Clock configuration
 * @{
 */
#ifndef CLOCK_CORECLOCK
#define CLOCK_CORECLOCK     MHZ(32)
#endif /* CLOCK_CORECLOCK */
/** @} */

/**
 * @name ADC Configuration
 *
 * @{
 */
#ifndef ADC_NUMOF
#define ADC_NUMOF           (0U)
#endif /* ADC_NUMOF */
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
#ifndef I2C_NUMOF
#define I2C_NUMOF           (0U)
#endif /* I2C_NUMOF */
/** @} */

/**
 * @name   PWM configuration
 *
 * @{
 */
#ifndef PWM_NUMOF
#define PWM_NUMOF           (0U)
#endif /* PWM_NUMOF */
/** @} */

/**
 * @name    SPI configuration
 *
 * The SS pin must be configured as output for the SPI device to work as
 * master correctly, though we do not use it for now (as we handle the chip
 * select externally for now)
 *
 * @{
 */
#ifndef SPI_NUMOF
#define SPI_NUMOF           (0U)
#endif /* SPI_NUMOF */
/** @} */

/**
 * @name    UART configuration
 *
 * The UART devices have fixed pin mappings, so all we need to do, is to specify
 * which devices we would like to use and their corresponding RX interrupts. See
 * the reference manual for the fixed pin mapping.
 *
 * @{
 */
#ifndef UART_NUMOF
#define UART_NUMOF          (0U)
#endif /* UART_NUMOF */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_COMMON_H */
/** @} */
