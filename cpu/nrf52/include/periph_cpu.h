/*
 * Copyright (C) 2015-2018 Freie Universität Berlin
 *               2020 Philipp-Alexander Blum <philipp-blum@jakiku.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf52
 * @{
 *
 * @file
 * @brief           nRF52 specific definitions for handling peripherals
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Philipp-Alexander Blum <philipp-blum@jakiku.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "periph_cpu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   System core clock speed, fixed to 64MHz for all NRF52x CPUs
 */
#define CLOCK_CORECLOCK     (64000000U)

/**
 * @name    Peripheral clock speed (fixed to 16MHz for nRF52 based CPUs)
 */
#define PERIPH_CLOCK        (16000000U)

/**
 * @brief   The nRF52 family of CPUs provides a fixed number of 9 ADC lines
 */
#ifdef SAADC_CH_PSELP_PSELP_VDDHDIV5
#define ADC_NUMOF           (10U)
#else
#define ADC_NUMOF           (9U)
#endif

/**
 * @brief   nRF52 specific naming of ADC lines (for convenience)
 */
enum {
    NRF52_AIN0 = 0,         /**< Analog Input 0 */
    NRF52_AIN1 = 1,         /**< Analog Input 1 */
    NRF52_AIN2 = 2,         /**< Analog Input 2 */
    NRF52_AIN3 = 3,         /**< Analog Input 3 */
    NRF52_AIN4 = 4,         /**< Analog Input 4 */
    NRF52_AIN5 = 5,         /**< Analog Input 5 */
    NRF52_AIN6 = 6,         /**< Analog Input 6 */
    NRF52_AIN7 = 7,         /**< Analog Input 7 */
    NRF52_VDD  = 8,         /**< VDD, not useful if VDD is reference... */
#ifdef SAADC_CH_PSELP_PSELP_VDDHDIV5
    NRF52_VDDHDIV5 = 9,     /**< VDDH divided by 5 */
#endif
};

#ifndef DOXYGEN
/**
 * @brief   Override ADC resolution values
 * @{
 */
#define HAVE_ADC_RES_T
typedef enum {
    ADC_RES_6BIT  = 0xf0,   /**< not supported by hardware */
    ADC_RES_8BIT  = 0x00,   /**< ADC resolution: 8 bit */
    ADC_RES_10BIT = 0x01,   /**< ADC resolution: 10 bit */
    ADC_RES_12BIT = 0x02,   /**< ADC resolution: 12 bit */
    ADC_RES_14BIT = 0xf1,   /**< supported with oversampling, not implemented */
    ADC_RES_16BIT = 0xf2    /**< not supported by hardware */
} adc_res_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE    (64)
#endif

/**
 * @brief Common SPI/I2C interrupt callback
 *
 * @param   arg     Opaque context pointer
 */
typedef void (*spi_twi_irq_cb_t)(void *arg);

/**
 * @brief Register a SPI IRQ handler for a shared I2C/SPI irq vector
 *
 * @param   bus bus to register the IRQ handler on
 * @param   cb  callback to call on IRQ
 * @param   arg Argument to pass to the handler
 */
void spi_twi_irq_register_spi(NRF_SPIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg);

/**
 * @brief Register a I2C IRQ handler for a shared I2C/SPI irq vector
 *
 * @param   bus bus to register the IRQ handler on
 * @param   cb  callback to call on IRQ
 * @param   arg Argument to pass to the handler
 */
void spi_twi_irq_register_i2c(NRF_TWIM_Type *bus,
                              spi_twi_irq_cb_t cb, void *arg);

/**
 * @brief   Acquire the shared I2C/SPI peripheral in I2C mode
 *
 * @param   bus bus to acquire exclusive access on
 * @param   cb  ISR handler to call on IRQ
 * @param   arg ISR handler argument
 */
void nrf5x_i2c_acquire(NRF_TWIM_Type *bus, spi_twi_irq_cb_t cb, void *arg);

/**
 * @brief   Release the shared I2C/SPI peripheral in I2C mode
 *
 * @param   bus bus to release exclusive access on
 */
void nrf5x_i2c_release(NRF_TWIM_Type *bus);

/**
 * @brief   Acquire the shared I2C/SPI peripheral in SPI mode
 *
 * @param   bus bus to release exclusive access on
 * @param   cb  ISR handler to call on IRQ
 * @param   arg ISR handler argument
 */
void nrf5x_spi_acquire(NRF_SPIM_Type *bus, spi_twi_irq_cb_t cb, void *arg);

/**
 * @brief   Acquire the shared I2C/SPI peripheral in SPI mode
 *
 * @param   bus bus to release exclusive access on
 */
void nrf5x_spi_release(NRF_SPIM_Type *bus);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
