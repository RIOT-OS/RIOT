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
     * Core:  48 MHz
     * Bus:   24 MHz
     * Flash: 24 MHz
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
    .fll_factor_fei     = KINETIS_MCG_FLL_FACTOR_1464, /* FEI FLL freq = 48 MHz */
    .fll_factor_fee     = KINETIS_MCG_FLL_FACTOR_1280, /* FEE FLL freq = 40 MHz */
};
/* Radio xtal frequency, either 32 MHz or 26 MHz */
#define CLOCK_RADIOXTAL              (32000000ul)
/* CPU core clock, the MCG clock output frequency */
#define CLOCK_CORECLOCK              (48000000ul)
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
    [ 0] = { ADC0, GPIO_UNDEF, 26 },       /* internal: temperature sensor */
    /* Note: the band gap buffer uses a bit of current and is turned off by default,
     * Set PMC->REGSC |= PMC_REGSC_BGBE_MASK before reading or the input will be floating */
    [ 1] = { ADC0, GPIO_UNDEF, 27 },       /* internal: band gap */
    [ 2] = { ADC0, GPIO_UNDEF, 29 },       /* internal: V_REFH */
    [ 3] = { ADC0, GPIO_UNDEF, 30 },       /* internal: V_REFL */
    [ 4] = { ADC0, GPIO_UNDEF, 23 },       /* internal: DCDC divided battery level */
    [ 5] = { ADC0, GPIO_UNDEF,  0 | ADC_SC1_DIFF_MASK }, /* ADC0_DP-ADC0_DM differential reading */
    [ 6] = { ADC0, GPIO_PIN(PORT_B,  3),  2 }, /* ADC0_SE2 */
    [ 7] = { ADC0, GPIO_PIN(PORT_B,  2),  3 }, /* ADC0_SE3 */
};
#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
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

/**
 * @name   SPI configuration
 *
 * Clock configuration values based on the configured 16Mhz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
* @{
*/
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(5) |          /* -> 100000Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(4) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(4) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(4)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(3) |          /* -> 400000Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(2) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(2) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(2)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) |          /* -> 1000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) |          /* -> 4000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(1) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(1) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(1)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(0) |          /* -> 4000000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(0)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_C, 18),
        .pin_mosi = GPIO_PIN(PORT_C, 17),
        .pin_clk  = GPIO_PIN(PORT_C, 16),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 19),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
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
/* This CPU has I2C0 clocked by the bus clock and I2C1 clocked by the system
 * clock. This causes trouble with the current implementation in kinetis
 * which only supports one set of frequency dividers at a time */
/* The current configuration sets the dividers so that the I2C0 bus will run at
 * half the requested speed, to avoid exceeding the requested speed on I2C1 with
 * the same configuration */
#define I2C_NUMOF                    (2U)
#define I2C_0_EN                     1
/* Disabled while waiting for a rewritten i2c driver which supports different
 * clock sources for each i2c module */
#define I2C_1_EN                     1
/* Low (10 kHz): MUL = 2, SCL divider = 1792, total: 3584 */
#define KINETIS_I2C_F_ICR_LOW        (0x3A)
#define KINETIS_I2C_F_MULT_LOW       (1)
/* Normal (100 kHz): MUL = 1, SCL divider = 320, total: 320 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x25)
#define KINETIS_I2C_F_MULT_NORMAL    (0)
/* Fast (400 kHz): MUL = 1, SCL divider = 80, total: 80 */
#define KINETIS_I2C_F_ICR_FAST       (0x14)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 32, total: 32 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x09)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)

/* I2C 0 device configuration */
#define I2C_0_DEV                    I2C0
#define I2C_0_CLKEN()                (bit_set32(&SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT))
#define I2C_0_CLKDIS()               (bit_clear32(&SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT))
#define I2C_0_IRQ                    I2C0_IRQn
#define I2C_0_IRQ_HANDLER            isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT                   PORTB
#define I2C_0_PORT_CLKEN()           (bit_set32(&SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT))
#define I2C_0_PIN_AF                 3
#define I2C_0_SDA_PIN                1
#define I2C_0_SCL_PIN                0
#define I2C_0_PORT_CFG               (PORT_PCR_MUX(I2C_0_PIN_AF))
/* I2C 1 device configuration */
#define I2C_1_DEV                    I2C1
#define I2C_1_CLKEN()                (bit_set32(&SIM->SCGC4, SIM_SCGC4_I2C1_SHIFT))
#define I2C_1_CLKDIS()               (bit_clear32(&SIM->SCGC4, SIM_SCGC4_I2C1_SHIFT))
#define I2C_1_IRQ                    I2C1_IRQn
#define I2C_1_IRQ_HANDLER            isr_i2c1
/* I2C 1 pin configuration */
#define I2C_1_PORT                   PORTC
#define I2C_1_PORT_CLKEN()           (bit_set32(&SIM->SCGC5, SIM_SCGC5_PORTC_SHIFT))
#define I2C_1_PIN_AF                 3
#define I2C_1_SDA_PIN                3
#define I2C_1_SCL_PIN                2
#define I2C_1_PORT_CFG               (PORT_PCR_MUX(I2C_0_PIN_AF))
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
