/*
 * Copyright (C) 2018 Eistec AB
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_phynode-kw41z
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the PhyNODE KW41Z
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      José Alamos <jose.alamos@haw-hamburg.de>
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
        .dev      = SPI1,
        .pin_miso = GPIO_PIN(PORT_A, 17),
        .pin_mosi = GPIO_PIN(PORT_A, 16),
        .pin_clk  = GPIO_PIN(PORT_A, 18),
        .pin_cs   = {
            GPIO_PIN(PORT_A, 19),
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
            SPI_CS_UNDEF,
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
    /* internal: temperature sensor */
    /* The temperature sensor has a very high output impedance, it must not be
     * sampled using hardware averaging, or the sampled values will be garbage */
    [ 0] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 26, .avg = ADC_AVG_NONE },
    /* Note: the band gap buffer uses a bit of current and is turned off by default,
     * Set PMC->REGSC |= PMC_REGSC_BGBE_MASK before reading or the input will be floating */
    /* internal: band gap */
    [ 1] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 27, .avg = ADC_AVG_MAX },
    /* internal: DCDC divided battery level */
    [ 2] = { .dev = ADC0, .pin = GPIO_UNDEF, .chan = 23, .avg = ADC_AVG_MAX },
};
#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * KW41Z ADC reference settings:
 * 0: VREFH external pin or VREF_OUT 1.2 V signal (if VREF module is enabled)
 * 1: VDDA (analog supply input voltage)
 * 2-3: reserved
 *
 * VREF_OUT and VREFH shares the pin on KW41Z and is tied to VDDA on the
 * USB-KW41Z board, so it does not matter which setting is chosen here for this
 * board.
 */
#define ADC_REF_SETTING     1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
