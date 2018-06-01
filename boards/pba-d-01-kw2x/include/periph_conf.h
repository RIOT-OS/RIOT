/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_pba-d-01-kw2x
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the phyWAVE-KW22 Board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
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
     * This configuration results in the system running from the PLL output with
     * the following clock frequencies:
     * Core:  48 MHz
     * Bus:   48 MHz
     * Flash: 24 MHz
     */
    .clkdiv1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(0) |
               SIM_CLKDIV1_OUTDIV4(1),
    .rtc_clc = 0, /* External load caps on the FRDM-K22F board */
    .osc32ksel = SIM_SOPT1_OSC32KSEL(2),
    .clock_flags =
        /* No OSC0_EN, use modem clock from EXTAL0 */
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        0,
    .default_mode = KINETIS_MCG_MODE_PEE,
    /* The modem generates a 4 MHz clock signal */
    .erc_range = KINETIS_MCG_ERC_RANGE_HIGH,
    .osc_clc = 0, /* OSC0 is unused*/
    .oscsel = MCG_C7_OSCSEL(0), /* Use EXTAL0 for external clock */
    .fcrdiv = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv = MCG_C1_FRDIV(0b010), /* Divide by 128 */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1280, /* FLL freq = 40 MHz */
    .pll_prdiv = MCG_C5_PRDIV0(0b00001), /* Divide by 2 */
    .pll_vdiv = MCG_C6_VDIV0(0b00000), /* Multiply by 24 => PLL freq = 48 MHz */
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
#define LPTMR_NUMOF             (0U)
#define LPTMR_CONFIG {}
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
        .dev    = UART2,
        .freq   = CLOCK_BUSCLOCK,
        .pin_rx = GPIO_PIN(PORT_D, 2),
        .pin_tx = GPIO_PIN(PORT_D, 3),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART2_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART2_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
    {
        .dev    = UART0,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_D, 6),
        .pin_tx = GPIO_PIN(PORT_D, 7),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART0_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    }
};

#define UART_0_ISR          (isr_uart2_rx_tx)
#define UART_1_ISR          (isr_uart0_rx_tx)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name    ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    { .dev = ADC0, .pin = GPIO_PIN(PORT_E, 2), .chan =  1 },
    { .dev = ADC0, .pin = GPIO_PIN(PORT_E, 3), .chan =  1 },
    { .dev = ADC0, .pin = GPIO_PIN(PORT_D, 7), .chan = 22 },
    { .dev = ADC0, .pin = GPIO_PIN(PORT_D, 5), .chan =  6 },
    { .dev = ADC0, .pin = GPIO_PIN(PORT_E, 0), .chan = 10 },
    { .dev = ADC0, .pin = GPIO_PIN(PORT_E, 1), .chan = 11 }
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/*
 * KW2xD ADC reference settings:
 * 0: VREFH/VREFL external pin pair
 * 1-3: reserved
 */
#define ADC_REF_SETTING     0
/** @} */

/**
 * @name    PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .ftm        = FTM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_A, 4), .af = 3, .ftm_chan = 1 },
            { .pin = GPIO_PIN(PORT_D, 4), .af = 4, .ftm_chan = 4 },
            { .pin = GPIO_PIN(PORT_D, 6), .af = 4, .ftm_chan = 6 },
            { .pin = GPIO_PIN(PORT_A, 1), .af = 3, .ftm_chan = 1 }
        },
        .chan_numof = 4,
        .ftm_num    = 0
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */


/**
 * @name    SPI device configuration
 *
 * Clock configuration values based on the configured 48Mhz module clock.
 *
 * Auto-generated by:
 * cpu/kinetis/dist/calc_spi_scalers/calc_spi_scalers.c
 *
 * @{
 */
static const uint32_t spi_clk_config[] = {
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(8) |          /* -> 93750Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(8) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(8) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(8)
    ),
    (
        SPI_CTAR_PBR(0) | SPI_CTAR_BR(6) |          /* -> 375000Hz */
        SPI_CTAR_PCSSCK(0) | SPI_CTAR_CSSCK(6) |
        SPI_CTAR_PASC(0) | SPI_CTAR_ASC(6) |
        SPI_CTAR_PDT(0) | SPI_CTAR_DT(6)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(4) |          /* -> 1000000Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(3) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(3) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(3)
    ),
    (
        SPI_CTAR_PBR(2) | SPI_CTAR_BR(0) |          /* -> 4800000Hz */
        SPI_CTAR_PCSSCK(2) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(2) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(2) | SPI_CTAR_DT(0)
    ),
    (
        SPI_CTAR_PBR(1) | SPI_CTAR_BR(0) |          /* -> 8000000Hz */
        SPI_CTAR_PCSSCK(1) | SPI_CTAR_CSSCK(0) |
        SPI_CTAR_PASC(1) | SPI_CTAR_ASC(0) |
        SPI_CTAR_PDT(1) | SPI_CTAR_DT(0)
    )
};

static const spi_conf_t spi_config[] = {
    {
        .dev      = SPI0,
        .pin_miso = GPIO_PIN(PORT_C, 7),
        .pin_mosi = GPIO_PIN(PORT_C, 6),
        .pin_clk  = GPIO_PIN(PORT_C, 5),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 4),
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
        .pin_miso = GPIO_PIN(PORT_B, 17),
        .pin_mosi = GPIO_PIN(PORT_B, 16),
        .pin_clk  = GPIO_PIN(PORT_B, 11),
        .pin_cs   = {
            GPIO_PIN(PORT_B, 10),
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
        .scl_pin = GPIO_PIN(PORT_E,  1),
        .sda_pin = GPIO_PIN(PORT_E,  0),
        .freq = CLOCK_BUSCLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C1_IRQn,
        .scl_pcr = (PORT_PCR_MUX(6) | PORT_PCR_ODE_MASK),
        .sda_pcr = (PORT_PCR_MUX(6) | PORT_PCR_ODE_MASK),
    },
};
#define I2C_NUMOF           (sizeof(i2c_config) / sizeof(i2c_config[0]))
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
