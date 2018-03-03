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
               SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(2),
    .default_mode = KINETIS_MCG_MODE_FEE,
    .erc_range = KINETIS_MCG_ERC_RANGE_LOW, /* Input clock is 32768 Hz */
    .fcrdiv = 0, /* Fast IRC divide by 1 => 4 MHz */
    .oscsel = 1, /* Use RTC for external clock */
    /* 16 pF capacitors yield ca 10 pF load capacitance as required by the
     * onboard xtal, not used when OSC0 is disabled */
    .clc = 0b0001,
    .fll_frdiv = 0b000, /* Divide by 1 => FLL input 32768 Hz */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    /* PLL is unavailable when using a 32768 Hz source clock, so the
     * configuration below can only be used if the above config is modified to
     * use the 16 MHz crystal instead of the RTC. */
    .pll_prdiv = 0b00111, /* Divide by 8 */
    .pll_vdiv = 0b01100, /* Multiply by 36 => PLL freq = 72 MHz */
    .enable_oscillator = false, /* the RTC module provides the clock input signal */
    .select_fast_irc = true, /* Only used for FBI mode */
    .enable_mcgirclk = false,
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
        .pin_rx = GPIO_PIN(PORT_A, 14),
        .pin_tx = GPIO_PIN(PORT_A, 15),
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

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* internal: temperature sensor */
    [ 0] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 26 },
    /* internal: band gap */
    [ 1] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 27 },
    /* internal: V_REFSH */
    [ 2] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 29 },
    /* internal: V_REFSL */
    [ 3] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 30 },
    /* internal: DAC0 module output level */
    [ 4] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 23 },
    /* internal: VREF module output level */
    [ 5] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 18 },
     /* on board connection to Mulle Vbat/2 on PGA1_DP pin */
    [ 6] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan =  0 },
    /* on board connection to Mulle Vchr/2 on PGA1_DM pin */
    [ 7] = { .dev = ADC1, .pin = GPIO_UNDEF,           .chan = 19 },
    /* expansion port PGA0_DP pin */
    [ 8] = { .dev = ADC0, .pin = GPIO_UNDEF,           .chan =  0 },
    /* expansion port PGA0_DM pin */
    [ 9] = { .dev = ADC0, .pin = GPIO_UNDEF,           .chan = 19 },
    /* expansion port PTA17 */
    [10] = { .dev = ADC1, .pin = GPIO_PIN(PORT_A, 17), .chan = 17 },
    /* expansion port PTB0  */
    [11] = { .dev = ADC1, .pin = GPIO_PIN(PORT_B,  0), .chan =  8 },
    /* expansion port PTC0  */
    [12] = { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  0), .chan = 14 },
    /* expansion port PTC8  */
    [13] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C,  8), .chan =  4 },
    /* expansion port PTC9  */
    [14] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C,  9), .chan =  5 },
    /* expansion port PTC10 */
    [15] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C, 10), .chan =  6 },
    /* expansion port PTC11 */
    [16] = { .dev = ADC1, .pin = GPIO_PIN(PORT_C, 11), .chan =  7 }
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
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

#define DAC_NUMOF           (sizeof(dac_config) / sizeof(dac_config[0]))
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

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
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
            GPIO_UNDEF
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

/** @} */

/**
 * @name I2C configuration
 * @{
 */
#define I2C_NUMOF               (1U)
#define I2C_0_EN                1
#define I2C_1_EN                0

/* I2C 0 device configuration */
#define I2C_0_DEV               I2C0
#define I2C_0_CLKEN()           (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 1)
#define I2C_0_CLKDIS()          (BITBAND_REG32(SIM->SCGC4, SIM_SCGC4_I2C0_SHIFT) = 0)
#define I2C_0_IRQ               I2C0_IRQn
#define I2C_0_IRQ_HANDLER       isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT              PORTB
#define I2C_0_PORT_CLKEN()      (BITBAND_REG32(SIM->SCGC5, SIM_SCGC5_PORTB_SHIFT) = 1)
#define I2C_0_PIN_AF            2
#define I2C_0_SDA_PIN           1
#define I2C_0_SCL_PIN           2
#define I2C_0_PORT_CFG          (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */

/**
 * @name I2C baud rate configuration
 * @{
 */
/* Low (10 kHz): MUL = 2, SCL divider = 2560, total: 5120 */
#define KINETIS_I2C_F_ICR_LOW        (0x3D)
#define KINETIS_I2C_F_MULT_LOW       (1)
/* Normal (100 kHz): MUL = 2, SCL divider = 240, total: 480 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1F)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 128, total: 128 */
#define KINETIS_I2C_F_ICR_FAST       (0x17)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 48, total: 48 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x10)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)
/** @} */

/**
 * @name RTC configuration
 * @{
 */
/* RIOT RTC implementation uses RTT for underlying timekeeper */
#define RTC_NUMOF           (1U)
/** @} */

/**
 * @name RTT configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_IRQ             RTC_IRQn
#define RTT_IRQ_PRIO        10
#define RTT_ISR             isr_rtc
#define RTT_DEV             RTC
#define RTT_UNLOCK()        (BITBAND_REG32(SIM->SCGC6, SIM_SCGC6_RTC_SHIFT) = 1)
#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_FREQUENCY       (1)             /* in Hz */

/**
 * RTC module crystal load capacitance configuration bits.
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

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
