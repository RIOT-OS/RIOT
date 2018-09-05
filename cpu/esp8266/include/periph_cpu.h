/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       CPU specific definitions and functions for peripheral handling
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <stdint.h>

#include "eagle_soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Override the ADC resolution configuration
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_10BIT      /* only one resolution is supported */
} adc_res_t;
/** @} */

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (4U)

/**
 * @name    GPIO configuration of ESP8266
 * @{
 */

/**
 * @brief   Available ports on the ESP8266
 * @{
 */
#define PORT_GPIO 0       /**< port GPIO */
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF (GPIO_ID_NONE)

/**
 * @brief   Define CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)  ((x << 4) | y)

/**
 * @brief   Define CPU specific number of GPIO pins
 */
#define GPIO_PIN_NUMOF  GPIO_PIN_COUNT+1

/**
 * @name   Predefined GPIO names
 * @{
 */
#define GPIO0       (GPIO_PIN(PORT_GPIO,0))
#define GPIO1       (GPIO_PIN(PORT_GPIO,1))
#define GPIO2       (GPIO_PIN(PORT_GPIO,2))
#define GPIO3       (GPIO_PIN(PORT_GPIO,3))
#define GPIO4       (GPIO_PIN(PORT_GPIO,4))
#define GPIO5       (GPIO_PIN(PORT_GPIO,5))
#define GPIO6       (GPIO_PIN(PORT_GPIO,6))
#define GPIO7       (GPIO_PIN(PORT_GPIO,7))
#define GPIO8       (GPIO_PIN(PORT_GPIO,8))
#define GPIO9       (GPIO_PIN(PORT_GPIO,9))
#define GPIO10      (GPIO_PIN(PORT_GPIO,10))
#define GPIO11      (GPIO_PIN(PORT_GPIO,11))
#define GPIO12      (GPIO_PIN(PORT_GPIO,12))
#define GPIO13      (GPIO_PIN(PORT_GPIO,13))
#define GPIO14      (GPIO_PIN(PORT_GPIO,14))
#define GPIO15      (GPIO_PIN(PORT_GPIO,15))
#define GPIO16      (GPIO_PIN(PORT_GPIO,16))
/** @} */

#ifndef DOXYGEN
#define GPIO0_MASK  (BIT(0))
#define GPIO1_MASK  (BIT(1))
#define GPIO2_MASK  (BIT(2))
#define GPIO3_MASK  (BIT(3))
#define GPIO4_MASK  (BIT(4))
#define GPIO5_MASK  (BIT(5))
#define GPIO6_MASK  (BIT(6))
#define GPIO7_MASK  (BIT(7))
#define GPIO8_MASK  (BIT(8))
#define GPIO9_MASK  (BIT(9))
#define GPIO10_MASK (BIT(10))
#define GPIO11_MASK (BIT(11))
#define GPIO12_MASK (BIT(12))
#define GPIO13_MASK (BIT(13))
#define GPIO14_MASK (BIT(14))
#define GPIO15_MASK (BIT(15))
#define GPIO16_MASK (BIT(16))

/**
 * @brief   Override flank selection values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_NONE    = 0,
    GPIO_RISING  = 1,        /**< emit interrupt on rising flank  */
    GPIO_FALLING = 2,        /**< emit interrupt on falling flank */
    GPIO_BOTH    = 3,        /**< emit interrupt on both flanks   */
    GPIO_LOW     = 4,        /**< emit interrupt on low level     */
    GPIO_HIGH    = 5         /**< emit interrupt on low level     */
} gpio_flank_t;
/** @} */
#endif /* DOXYGEN */

/** @} */

/**
 * @name   I2C configuration
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @name    Power management configuration
 * @{
 */
#define PROVIDES_PM_SET_LOWEST
#define PROVIDES_PM_RESTART
#define PROVIDES_PM_OFF
/** @} */

/**
 * @name   SPI configuration
 * @{
 */
#if defined(MODULE_PERIPH_SPI)

#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS

#endif /* MODULE_PERIPH_SPI */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
