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

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Clock system configuration
 * @{
 */
static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running with the internal clock
     * with the following clock frequencies:
     * Core:  41.94 MHz
     * Bus:   20.97 MHz
     * Flash: 20.97 MHz
     */
    .clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(1),
    /* unsure if this RTC load cap configuration is correct, but it matches the
     * settings used by the example code in the NXP provided SDK */
    .rtc_clc            = 0,
    /* Use the 32 kHz oscillator as ERCLK32K. Note that the values here have a
     * different mapping for the KW41Z than the values used in the Kinetis K series */
    .osc32ksel          = SIM_SOPT1_OSC32KSEL(0),
    .clock_flags =
        KINETIS_CLOCK_OSC0_EN | /* Enable RSIM oscillator */
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
        KINETIS_CLOCK_MCGIRCLK_STOP_EN |
        0,
    /* Using FEI mode by default, the external crystal settings below are only
     * used if mode is changed to an external mode (PEE, FBE, or FEE) */
    .default_mode       = KINETIS_MCG_MODE_FEI,
    /* The crystal connected to RSIM OSC is 32 MHz */
    .erc_range          = KINETIS_MCG_ERC_RANGE_VERY_HIGH,
    .osc_clc            = 0, /* no load cap configuration */
    .oscsel             = MCG_C7_OSCSEL(0), /* Use RSIM for external clock */
    .fcrdiv             = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv          = MCG_C1_FRDIV(0b101), /* Divide by 1024 */
    .fll_factor_fei     = KINETIS_MCG_FLL_FACTOR_1280, /* FEI FLL freq = 41.94 MHz */
    .fll_factor_fee     = KINETIS_MCG_FLL_FACTOR_1280, /* FEE FLL freq = 40 MHz */
};
/* Radio xtal frequency, either 32 MHz or 26 MHz */
#define CLOCK_RADIOXTAL              (32000000ul)
/* CPU core clock, the MCG clock output frequency */
#define CLOCK_CORECLOCK              (32768 * 1280)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 2)
#define CLOCK_MCGIRCLK               (4000000ul)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (1U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
    }
#define LPTMR_NUMOF             (1U)
#define LPTMR_CONFIG { \
        { \
            .dev = LPTMR0, \
            .irqn = LPTMR0_IRQn, \
            .src = 2, \
            .base_freq = 32768u, \
        } \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))
#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define LPTMR_ISR_0             isr_lptmr0
/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = LPUART0,
        .freq   = CLOCK_MCGIRCLK,
        .pin_rx = GPIO_PIN(PORT_C,  6),
        .pin_tx = GPIO_PIN(PORT_C,  7),
        .pcr_rx = PORT_PCR_MUX(4),
        .pcr_tx = PORT_PCR_MUX(4),
        .irqn   = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC5,
        .scgc_bit = SIM_SCGC5_LPUART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_LPUART,
    },
};
#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
#define LPUART_0_ISR        isr_lpuart0
/* Use MCGIRCLK (internal reference 4 MHz clock) */
#define LPUART_0_SRC        3
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
#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
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

/**
 * @name   SPI configuration
 *
 * Clock configuration values based on the configured 20.97 MHz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
* @{
*/
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(3) | SPI_CTAR_BR(5) |          /* -> 93622Hz */
        SPI_CTAR_PCSSCK(3) | SPI_CTAR_CSSCK(4) |
        SPI_CTAR_PASC(3) | SPI_CTAR_ASC(4) |
        SPI_CTAR_PDT(3) | SPI_CTAR_DT(4)
    ),
    (
        SPI_CTAR_PBR(3) | SPI_CTAR_BR(3) |          /* -> 374491Hz */
        SPI_CTAR_PCSSCK(3) | SPI_CTAR_CSSCK(2) |
        SPI_CTAR_PASC(3) | SPI_CTAR_ASC(2) |
        SPI_CTAR_PDT(3) | SPI_CTAR_DT(2)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(3) |          /* -> 873813Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(2) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(2) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(2)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) |          /* -> 3495253Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(0)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) |          /* -> 5242880Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(1) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(1) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(1)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI1,
        .pin_miso = GPIO_PIN(PORT_A, 17),
        .pin_mosi = GPIO_PIN(PORT_A, 16),
        .pin_clk  = GPIO_PIN(PORT_A, 18),
        .pin_cs   = {
            GPIO_PIN(PORT_A, 19),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI1_MASK
    }
};
#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */


/**
* @name I2C configuration
* @{
*/
static const i2c_conf_t i2c_config[] = {
    {
        .i2c = I2C1,
        .scl_pin = GPIO_PIN(PORT_C, 2),
        .sda_pin = GPIO_PIN(PORT_C, 3),
        .freq = CLOCK_CORECLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C1_IRQn,
        .scl_pcr = (PORT_PCR_MUX(3)),
        .sda_pcr = (PORT_PCR_MUX(3)),
    },
};
#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
#define I2C_0_ISR           (isr_i2c1)
/** @} */

/**
 * @name Random Number Generator configuration
 * @{
 */
#define KINETIS_TRNG                TRNG
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
