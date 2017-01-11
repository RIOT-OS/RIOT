/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
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
/** @} */

/**
 * @brief   Define a custom GPIO_PIN macro
 *
 * For the CC2538, we use OR the gpio ports base register address with the
 * actual pin number.
 */
#define GPIO_PIN(port, pin) (gpio_t)(((uint32_t)GPIO_A + (port << 12)) | pin)

/**
 * @brief declare needed generic SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Override the default GPIO mode settings
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = ((uint8_t)0x00),               /**< input, no pull */
    GPIO_IN_PD = ((uint8_t)IOC_OVERRIDE_PDE),   /**< input, pull-down */
    GPIO_IN_PU = ((uint8_t)IOC_OVERRIDE_PUE),   /**< input, pull-up */
    GPIO_OUT   = ((uint8_t)IOC_OVERRIDE_OE),    /**< output */
    GPIO_OD    = (0xff),                        /**< not supported */
    GPIO_OD_PU = (0xff)                         /**< not supported */
} gpio_mode_t;
/** @} */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    gpio_t scl_pin;         /**< pin used for SCL */
    gpio_t sda_pin;         /**< pin used for SDA */
} i2c_conf_t;

/**
 * @brief   SPI configuration data structure
 * @{
 */
typedef struct {
    cc2538_ssi_t *dev;      /**< SSI device */
    gpio_t mosi_pin;        /**< pin used for MOSI */
    gpio_t miso_pin;        /**< pin used for MISO */
    gpio_t sck_pin;         /**< pin used for SCK */
    gpio_t cs_pin;          /**< pin used for CS */
} periph_spi_conf_t;
/** @} */

/**
 * @brief   Timer configuration data
 */
typedef struct {
    cc2538_gptimer_t *dev;  /**< timer device */
    uint_fast8_t channels;  /**< number of channels */
    uint_fast8_t cfg;       /**< timer config word */
} timer_conf_t;

#ifdef __cplusplus
}
#endif

#include "periph/dev_enums.h"

#endif /* PERIPH_CPU_H_ */
/** @} */
