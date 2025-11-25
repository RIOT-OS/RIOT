/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-kw41z
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "periph_conf_common.h"
#include "cfg_i2c_default.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_C, 18),
        .pin_mosi = GPIO_PIN(PORT_C, 17),
        .pin_clk  = GPIO_PIN(PORT_C, 16),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 19),
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI0_MASK
    },
    {
        .dev      = SPI1,
        .pin_miso = GPIO_PIN(PORT_A, 17),
        .pin_mosi = GPIO_PIN(PORT_A, 16),
        .pin_clk  = GPIO_PIN(PORT_A, 18),
        .pin_cs   = {
            GPIO_PIN(PORT_A, 19),
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI1_MASK
    }
};
#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* dev, pin, channel */
    /* ADC0_DP-ADC0_DM differential reading (Arduino A5 - A0) */
    [ 0] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan =  0 | ADC_SC1_DIFF_MASK, .avg = ADC_AVG_MAX },
    /* ADC0_DP single ended reading (Arduino A5) */
    [ 1] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan =  0, .avg = ADC_AVG_MAX },
    /* PTB2 (Arduino A2) */
    [ 2] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  2), .chan =  3, .avg = ADC_AVG_MAX },
    /* PTB3 (Arduino A3) */
    [ 3] = { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  3), .chan =  2, .avg = ADC_AVG_MAX },
    /* internal: temperature sensor */
    /* The temperature sensor has a very high output impedance, it must not be
     * sampled using hardware averaging, or the sampled values will be garbage */
    [ 4] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 26, .avg = ADC_AVG_NONE },
    /* Note: the band gap buffer uses a bit of current and is turned off by default,
     * Set PMC->REGSC |= PMC_REGSC_BGBE_MASK before reading or the input will be floating */
    /* internal: band gap */
    [ 5] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 27, .avg = ADC_AVG_MAX },
    /* internal: DCDC divided battery level */
    [ 6] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 23, .avg = ADC_AVG_MAX },
};
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * KW41Z ADC reference settings:
 * 0: VREFH external pin or VREF_OUT 1.2 V signal (if VREF module is enabled)
 * 1: VDDA (analog supply input voltage)
 * 2-3: reserved
 *
 * VREF_OUT and VREFH shares the pin on KW41Z and is only connected to a 100 nF
 * capacitor on the FRDM-KW41Z board. So use VDDA by default on this board
 * unless the application enables the VREF module.
 */
#define ADC_REF_SETTING     1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
