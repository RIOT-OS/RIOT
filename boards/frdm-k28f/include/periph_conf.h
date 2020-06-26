/*
 * Copyright (C) 2020 Koen Zandberg
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k28f
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-K28F
 *
 * @author      Koen Zandberg <koen@bergzand.net>
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
     * This configuration results in the system running from the FLL output with
     * the following clock frequencies:
     * Core:  150 MHz
     * Bus:   75 MHz
     * Flex:  75 MHz
     * Flash: 25 MHz
     */
    .clkdiv1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) |
               SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(5),
    .clkdiv3 = SIM_CLKDIV3_PLLFLLDIV(1) | SIM_CLKDIV3_PLLFLLFRAC(0),
    .periph_pllfllsel = SIM_SOPT2_PLLFLLSEL(1),
    .rtc_clc = 0, /* External load caps on the FRDM-K28F board */
    .osc32ksel = SIM_SOPT1_OSC32KSEL(2),
    .clock_flags =
        KINETIS_CLOCK_OSC0_EN |
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        0,
    .default_mode = KINETIS_MCG_MODE_PEE,
    /* The crystal connected to OSC0 is 12 MHz */
    .erc_range = KINETIS_MCG_ERC_RANGE_VERY_HIGH,
    .osc_clc = 0,                                   /* External load caps on the FRDM-K28F board */
    .oscsel = MCG_C7_OSCSEL(0),                     /* Use OSC0 for external clock */
    .fcrdiv = MCG_SC_FCRDIV(0),                     /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv = MCG_C1_FRDIV(0b011),               /* Divide by 256 */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464,  /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1920,  /* FLL freq = 60 MHz */
    .pll_prdiv = MCG_C5_PRDIV0(0b00000),            /* Divide by 1 */
    /* Multiply by 20 => PLL freq = 240 MHz => 120 * 2 */
    .pll_vdiv = MCG_C6_VDIV0(0b00100),
};
#define CLOCK_CORECLOCK              (120000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 2)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (2U)
#define PIT_CONFIG {             \
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
#define LPTMR_CONFIG {          \
        {                           \
            .dev = LPTMR0,          \
            .irqn = LPTMR0_LPTMR1_IRQn,    \
            .src = 2,               \
            .base_freq = 32768u,    \
        },                          \
}
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define PIT_ISR_0               isr_pit1
#define PIT_ISR_1               isr_pit3
#define LPTMR_ISR_0             isr_lptmr0_lptmr1
/** @} */

/**
 * @name UART configuration
 * @{
 */
#define LPUART_0_SRC    0x01

static const uart_conf_t uart_config[] = {
    {
        .dev = LPUART0,
        .freq = CLOCK_BUSCLOCK,
        .pin_rx = GPIO_PIN(PORT_C, 25),
        .pin_tx = GPIO_PIN(PORT_C, 24),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC2,
        .scgc_bit = SIM_SCGC2_LPUART0_SHIFT,
        .mode = UART_MODE_8N1,
        .type = KINETIS_LPUART,
    },
    {
        .dev = LPUART2,
        .freq = CLOCK_BUSCLOCK,
        .pin_rx = GPIO_PIN(PORT_A,  25),
        .pin_tx = GPIO_PIN(PORT_A,  24),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn = LPUART2_IRQn,
        .scgc_addr = &SIM->SCGC2,
        .scgc_bit = SIM_SCGC2_LPUART2_SHIFT,
        .mode = UART_MODE_8N1,
        .type = KINETIS_LPUART,
    },
};

#define LPUART_0_ISR          (isr_lpuart0)
#define LPUART_1_ISR          (isr_lpuart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* dev, pin, channel */
    [ 0] = { /* ADC0_DP0 */
        .dev = ADC0,
        .pin = GPIO_UNDEF,
        .chan =  0,
        .avg = ADC_AVG_MAX
    },
    [ 1] = { /* ADC0_DM0 */
        .dev = ADC0,
        .pin = GPIO_UNDEF,
        .chan = 19,
        .avg = ADC_AVG_MAX
    },
    [ 2] = { /* PTA7 (Arduino A0) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_A,  7),
        .chan = 10,
        .avg = ADC_AVG_MAX
    },
    [ 3] = { /* PTA8 (Arduino A1) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_A, 8),
        .chan = 11,
        .avg = ADC_AVG_MAX
    },
    [ 4] = { /* PTD1 (Arduino A2) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_D,  1),
        .chan =  5,
        .avg = ADC_AVG_MAX
    },
    [ 5] = { /* PTD6 (Arduino A3) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_D,  6),
        .chan =  7,
        .avg = ADC_AVG_MAX
    },
    [ 6] = { /* PTB2 (Arduino A4) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_B,  2),
        .chan = 12,
        .avg = ADC_AVG_MAX
    },
    [ 7] = { /* PTB3 (Arduino A5) */
        .dev = ADC0,
        .pin = GPIO_PIN(PORT_B,  3),
        .chan = 13,
        .avg = ADC_AVG_MAX
    },
    /* internal: temperature sensor */
    /* The temperature sensor has a very high output impedance, it must not be
     * sampled using hardware averaging, or the sampled values will be garbage */
    [ 8] = {
        .dev = ADC0,
        .pin = GPIO_UNDEF,
        .chan = 26,
        .avg = ADC_AVG_NONE
    },
    /* internal: band gap */
    /* Note: the band gap buffer uses a bit of current and is turned off by default,
     * Set PMC->REGSC |= PMC_REGSC_BGBE_MASK before reading or the input will be floating */
    [ 9] = {
        .dev = ADC0,
        .pin = GPIO_UNDEF,
        .chan = 27,
        .avg = ADC_AVG_MAX
    },
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * TODO: check
 * K28F ADC reference settings:
 * 0: VREFH/VREFL external pin pair
 * 1: VREF_OUT internal 1.2 V reference (VREF module must be enabled)
 * 2-3: reserved
 */
#define ADC_REF_SETTING     0
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .ftm = FTM0,
        .chan = {
            { .pin = GPIO_PIN(PORT_A, 1), .af = 3, .ftm_chan = 6 },
            { .pin = GPIO_PIN(PORT_A, 2), .af = 3, .ftm_chan = 7 },
            { .pin = GPIO_PIN(PORT_D, 5), .af = 4, .ftm_chan = 5 },
        },
        .chan_numof = 3,
        .ftm_num = 0
    }
};

#define PWM_NUMOF           ARRAY_SIZE(pwm_config)
/** @} */


/**
 * @name   SPI configuration
 *
 * Clock configuration values based on the configured 75Mhz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(8) |          /* -> 97656Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(7) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(7) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(7)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(6) |          /* -> 390625Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(5) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(5) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(5)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(4) |          /* -> 937500Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(3) |          /* -> 4687500Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(1) |          /* -> 9375000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(2) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(2) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(2)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev = SPI2,
        .pin_miso = GPIO_PIN(PORT_D, 14),
        .pin_mosi = GPIO_PIN(PORT_D, 13),
        .pin_clk = GPIO_PIN(PORT_D, 12),
        .pin_cs = {
            GPIO_PIN(PORT_D, 15),
            GPIO_PIN(PORT_D, 11),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr = GPIO_AF_2,
        .simmask = SIM_SCGC3_SPI2_MASK
    },
    {
        .dev = SPI1,
        .pin_miso = GPIO_PIN(PORT_E, 4),
        .pin_mosi = GPIO_PIN(PORT_E, 2),
        .pin_clk = GPIO_PIN(PORT_E, 1),
        .pin_cs = {
            GPIO_PIN(PORT_E, 5),
            GPIO_PIN(PORT_E, 0),
            GPIO_PIN(PORT_E, 3),
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr = GPIO_AF_2,
        .simmask = SIM_SCGC6_SPI1_MASK
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
        .i2c = I2C3,
        .scl_pin = GPIO_PIN(PORT_C, 29),
        .sda_pin = GPIO_PIN(PORT_C, 28),
        .freq = CLOCK_BUSCLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C3_IRQn,
        .scl_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
        .sda_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
    },
    {
        .i2c = I2C0,
        .scl_pin = GPIO_PIN(PORT_B, 2),
        .sda_pin = GPIO_PIN(PORT_B, 3),
        .freq = CLOCK_BUSCLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C0_IRQn,
        .scl_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
        .sda_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           (isr_i2c3)
#define I2C_1_ISR           (isr_i2c0)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
