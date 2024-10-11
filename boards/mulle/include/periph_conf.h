/*
 * Copyright (C) 2015 Eistec AB
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_mulle
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the Eistec Mulle
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
/* The crystal on the Mulle is designed for 12.5 pF load capacitance. According
 * to the data sheet, the K60 will have a 5 pF parasitic capacitance on the
 * XTAL32/EXTAL32 connection. The board traces might give some minor parasitic
 * capacitance as well. */
/* Use the equation
 * CL = (C1 * C2) / (C1 + C2) + Cstray
 * with C1 == C2:
 * C1 = 2 * (CL - Cstray)
 */
/* enable 14pF load capacitor which will yield a crystal load capacitance of 12 pF */
#define RTC_LOAD_CAP_BITS   (RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK | RTC_CR_SC2P_MASK)

static const clock_config_t clock_config = {
    /*
     * This configuration results in the system running from the FLL output with
     * the following clock frequencies:
     * Core:  48 MHz
     * Bus:   48 MHz
     * Flex:  24 MHz
     * Flash: 24 MHz
     */
    /* The board has a 16 MHz crystal, though it is not used in this configuration */
    /* This configuration uses the RTC crystal to provide the base clock, it
     * should have better accuracy than the internal slow clock, and lower power
     * consumption than using the 16 MHz crystal and the OSC0 module */
    .clkdiv1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(0) |
               SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(1),
    .rtc_clc = RTC_LOAD_CAP_BITS,
    .osc32ksel = SIM_SOPT1_OSC32KSEL(2),
    .clock_flags =
        /* no OSC0_EN, the RTC module provides the clock input signal for the FLL */
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        0,
    .default_mode = KINETIS_MCG_MODE_FEE,
    .erc_range = KINETIS_MCG_ERC_RANGE_LOW, /* Input clock is 32768 Hz */
    /* 16 pF capacitors yield ca 10 pF load capacitance as required by the
     * onboard xtal, not used when OSC0 is disabled */
    .osc_clc = OSC_CR_SC16P_MASK,
    .oscsel = MCG_C7_OSCSEL(1), /* Use RTC for external clock */
    .fcrdiv = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv = MCG_C1_FRDIV(0b000), /* Divide by 1 => FLL input 32768 Hz */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    /* PLL is unavailable when using a 32768 Hz source clock, so the
     * configuration below can only be used if the above config is modified to
     * use the 16 MHz crystal instead of the RTC. */
    .pll_prdiv = MCG_C5_PRDIV0(0b00111), /* Divide by 8 */
    .pll_vdiv = MCG_C6_VDIV0(0b01100), /* Multiply by 36 => PLL freq = 72 MHz */
};
#define CLOCK_CORECLOCK              (48000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (2U)
#define PIT_CONFIG {                 \
        {                            \
            .prescaler_ch = 0,       \
            .count_ch = 1,           \
        },                           \
        {                            \
            .prescaler_ch = 2,       \
            .count_ch = 3,           \
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
#define PIT_ISR_0               isr_pit1
#define PIT_ISR_1               isr_pit3
#define LPTMR_ISR_0             isr_lptmr0

/** @} */

/**
 * @name UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev    = UART0,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_A, 15),
        .pin_tx = GPIO_PIN(PORT_A, 14),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART0_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
    {
        .dev    = UART1,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_C, 3),
        .pin_tx = GPIO_PIN(PORT_C, 4),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART1_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART1_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
};

#define UART_0_ISR          (isr_uart0_rx_tx)
#define UART_1_ISR          (isr_uart1_rx_tx)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* internal: temperature sensor */
    /* The temperature sensor has a very high output impedance, it must not be
     * sampled using hardware averaging, or the sampled values will be garbage */
    [ 0] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 26, .avg = ADC_AVG_NONE },
    /* internal: band gap */
    [ 1] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 27, .avg = ADC_AVG_MAX },
    /* internal: V_REFSH */
    [ 2] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 29, .avg = ADC_AVG_MAX },
    /* internal: V_REFSL */
    [ 3] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 30, .avg = ADC_AVG_MAX },
    /* internal: DAC0 module output level */
    [ 4] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 23, .avg = ADC_AVG_MAX },
    /* internal: VREF module output level */
    [ 5] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 18, .avg = ADC_AVG_MAX },
    /* on board connection to Mulle Vbat/2 on PGA1_DP pin */
    [ 6] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan =  0, .avg = ADC_AVG_MAX },
    /* on board connection to Mulle Vchr/2 on PGA1_DM pin */
    [ 7] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 19, .avg = ADC_AVG_MAX },
    /* expansion port PGA0_DP pin */
    [ 8] = { .dev = ADC0, .pin = GPIO_UNDEF,           .chan =  0, .avg = ADC_AVG_MAX },
    /* expansion port PGA0_DM pin */
    [ 9] = { .dev = ADC0, .pin = GPIO_UNDEF,           .chan = 19, .avg = ADC_AVG_MAX },
    /* expansion port PTA17 */
    [10] = { .dev = ADC1, .pin = GPIO_PIN(PORT_A, 17), .chan = 17, .avg = ADC_AVG_MAX },
    /* expansion port PTB0  */
    [11] = { .dev = ADC1, .pin = GPIO_PIN(PORT_B,  0), .chan =  8, .avg = ADC_AVG_MAX },
    /* expansion port PTC0  */
    [12] = { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  0), .chan = 14, .avg = ADC_AVG_MAX },
    /* expansion port PTC8  */
    [13] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C,  8), .chan =  4, .avg = ADC_AVG_MAX },
    /* expansion port PTC9  */
    [14] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C,  9), .chan =  5, .avg = ADC_AVG_MAX },
    /* expansion port PTC10 */
    [15] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C, 10), .chan =  6, .avg = ADC_AVG_MAX },
    /* expansion port PTC11 */
    [16] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C, 11), .chan =  7, .avg = ADC_AVG_MAX },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * K60D ADC reference settings:
 * 0: VREFH/VREFL external pin pair
 * 1: VREF_OUT internal 1.2 V reference (VREF module must be enabled)
 * 2-3: reserved
 */
#define ADC_REF_SETTING     0
/** @} */

/**
 * @name    DAC configuration
 * @{
 */
static const dac_conf_t dac_config[] = {
    {
        .dev       = DAC0,
        .scgc_addr = &SIM->SCGC2,
        .scgc_bit  = SIM_SCGC2_DAC0_SHIFT
    }
};

#define DAC_NUMOF           ARRAY_SIZE(dac_config)
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .ftm        = FTM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_C, 1), .af = 4, .ftm_chan = 0 },
            { .pin = GPIO_PIN(PORT_C, 2), .af = 4, .ftm_chan = 1 },
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,          .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 0
    },
    {
        .ftm        = FTM1,
        .chan       = {
            { .pin = GPIO_PIN(PORT_A, 12), .af = 3, .ftm_chan = 0 },
            { .pin = GPIO_PIN(PORT_A, 13), .af = 3, .ftm_chan = 1 },
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 },
            { .pin = GPIO_UNDEF,           .af = 0, .ftm_chan = 0 }
        },
        .chan_numof = 2,
        .ftm_num    = 1
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name SPI configuration
 *
 * Clock configuration values based on the configured 47988736Hz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(8) |          /* -> 93728Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(8) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(8) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(8)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(6) |          /* -> 374912Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(6) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(6) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(6)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(4) |          /* -> 999765Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) |          /* -> 4798873Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(0)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) |          /* -> 7998122Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(0)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_D, 3),
        .pin_mosi = GPIO_PIN(PORT_D, 2),
        .pin_clk  = GPIO_PIN(PORT_D, 1),
        .pin_cs   = {
            GPIO_PIN(PORT_D, 0),
            GPIO_PIN(PORT_D, 4),
            GPIO_PIN(PORT_D, 5),
            GPIO_PIN(PORT_D, 6),
            SPI_CS_UNDEF,
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI0_MASK
    },
    {
        .dev      = SPI1,
        .pin_miso = GPIO_PIN(PORT_E, 3),
        .pin_mosi = GPIO_PIN(PORT_E, 1),
        .pin_clk  = GPIO_PIN(PORT_E, 2),
        .pin_cs   = {
            GPIO_PIN(PORT_E, 4),
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
 * @name I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .i2c = I2C0,
        .scl_pin = GPIO_PIN(PORT_B,  2),
        .sda_pin = GPIO_PIN(PORT_B,  1),
        .freq = CLOCK_BUSCLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C0_IRQn,
        .scl_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
        .sda_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           (isr_i2c0)
#define I2C_1_ISR           (isr_i2c1)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CONF_H */
