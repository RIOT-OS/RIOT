/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf51
 * @{
 *
 * @file
 * @brief           nRF51 specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 16MHz for all NRF51x CPUs
 */
#define CLOCK_CORECLOCK     (16000000U)

/**
 * @brief   Redefine some peripheral names to unify them between nRF51 and 52
 * @{
 */
#define SPI_SCKSEL          (dev(bus)->PSELSCK)
#define SPI_MOSISEL         (dev(bus)->PSELMOSI)
#define SPI_MISOSEL         (dev(bus)->PSELMISO)
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override I2C speed settings
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 0x01,                         /**< not supported */
    I2C_SPEED_NORMAL    = TWI_FREQUENCY_FREQUENCY_K100, /**< 100kbit/s */
    I2C_SPEED_FAST      = TWI_FREQUENCY_FREQUENCY_K400, /**< 400kbit/s */
    I2C_SPEED_FAST_PLUS = 0x02,                         /**< not supported */
    I2C_SPEED_HIGH      = 0x03,                         /**< not supported */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< ADC resolution: 6 bit (not supported) */
    ADC_RES_8BIT  = 0x00,   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x02,   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0xf1,   /**< ADC resolution: 12 bit (not supported) */
    ADC_RES_14BIT = 0xf2,   /**< ADC resolution: 14 bit (not supported) */
    ADC_RES_16BIT = 0xf3    /**< ADC resolution: 16 bit (not supported) */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief ADC configuration wrapper
 */
typedef gpio_t adc_conf_t;

/**
 * @brief   I2C (TWI) configuration options
 */
typedef struct {
    NRF_TWI_Type *dev;          /**< hardware device */
    gpio_t pin_scl;             /**< SCL pin */
    gpio_t pin_sda;             /**< SDA pin */
    uint8_t ppi;                /**< PPI channel to use */
    i2c_speed_t speed;          /**< bus speed */
} i2c_conf_t;

/**
 * @brief  SPI configuration values
 */
typedef struct {
    NRF_SPI_Type *dev;  /**< SPI device used */
    gpio_t sclk;        /**< CLK pin */
    gpio_t mosi;        /**< MOSI pin */
    gpio_t miso;        /**< MISO pin */
} spi_conf_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
