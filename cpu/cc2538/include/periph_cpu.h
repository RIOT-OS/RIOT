/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include <stdint.h>

#include "cc2538_gptimer.h"
#include "cc2538_ssi.h"
#include "cc2538_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (8U)

/**
 * @brief   Define a custom type for GPIO pins
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO_PIN(port_num, bit_num) GPIO_PXX_TO_NUM(port_num, bit_num)
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

typedef struct {
    gpio_t scl_pin;         /**< pin used for SCL */
    gpio_t sda_pin;         /**< pin used for SDA */
} i2c_conf_t;

/**
 * @brief   SPI configuration data structure
 */
#define HAVE_PERIPH_SPI_CONF_T
typedef struct {
    cc2538_ssi_t *dev;      /**< SSI device */
    gpio_t mosi_pin;        /**< pin used for MOSI */
    gpio_t miso_pin;        /**< pin used for MISO */
    gpio_t sck_pin;         /**< pin used for SCK */
    gpio_t cs_pin;          /**< pin used for CS */
} periph_spi_conf_t;

/**
 * @brief   Timer configuration data
 */
typedef struct {
    cc2538_gptimer_t *dev;  /**< timer device */
    uint_fast8_t channels;  /**< number of channels */
    uint_fast8_t cfg;       /**< timer config word */
} timer_conf_t;

/**
 * @brief   Override resolution options
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xa00,          /**< not supported by hardware */
    ADC_RES_7BIT  = (0 << 4),       /**< ADC resolution: 7 bit */
    ADC_RES_8BIT  = 0xb00,          /**< not supported by hardware */
    ADC_RES_9BIT  = (1 << 4),       /**< ADC resolution: 9 bit */
    ADC_RES_10BIT = (2 << 4),       /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = (3 << 4),       /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xc00,          /**< not supported by hardware */
    ADC_RES_16BIT = 0xd00,          /**< not supported by hardware */
} adc_res_t;

/**
 * @brief Masks for getting data
 * @{
 */
#define SOCADC_7_BIT_RSHIFT        9 /**< Mask for getting data( 7 bits ENOB) */
#define SOCADC_9_BIT_RSHIFT        7 /**< Mask for getting data( 9 bits ENOB) */
#define SOCADC_10_BIT_RSHIFT       6 /**< Mask for getting data(10 bits ENOB) */
#define SOCADC_12_BIT_RSHIFT       4 /**< Mask for getting data(12 bits ENOB) */
/** @} */

#ifdef __cplusplus
}
#endif

#include "periph/dev_enums.h"

#endif /* PERIPH_CPU_H_ */
/** @} */
