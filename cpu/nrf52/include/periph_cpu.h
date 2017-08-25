/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
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
 * @brief   Redefine some peripheral names to unify them between nRF51 and 52
 * @{
 */
#define UART_IRQN           (UARTE0_UART0_IRQn)
#define SPI_SCKSEL          (dev(bus)->PSEL.SCK)
#define SPI_MOSISEL         (dev(bus)->PSEL.MOSI)
#define SPI_MISOSEL         (dev(bus)->PSEL.MISO)
/** @} */

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
    ADC_RES_14BIT = 0xf1,   /**< supported with oversampling */
    ADC_RES_16BIT = 0xf2    /**< not supported by hardware */
} adc_res_t;
/** @} */

/**
 * @name    ADC configuration, valid for all boards using this CPU
 *
 * The NRF52832 has a fixed mapping of ADC pins and a fixed number of ADC channels,
 * so this ADC configuration is valid for all boards using this CPU. No need for
 * any board specific configuration.
 */
#define ADC_NUMOF           (8U)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
