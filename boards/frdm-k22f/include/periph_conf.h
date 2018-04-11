/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-k22f
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-K22F
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
     * This configuration results in the system running from the FLL output with
     * the following clock frequencies:
     * Core:  60 MHz
     * Bus:   30 MHz
     * Flex:  20 MHz
     * Flash: 20 MHz
     */
    .clkdiv1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) |
               SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(2),
    .rtc_clc = 0, /* External load caps on the FRDM-K22F board */
    .osc32ksel = SIM_SOPT1_OSC32KSEL(2),
    .clock_flags =
        KINETIS_CLOCK_OSC0_EN |
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        0,
    .default_mode = KINETIS_MCG_MODE_FEE,
    /* The crystal connected to OSC0 is 8 MHz */
    .erc_range = KINETIS_MCG_ERC_RANGE_HIGH,
    .osc_clc = 0, /* External load caps on the FRDM-K22F board */
    .oscsel = MCG_C7_OSCSEL(0), /* Use OSC0 for external clock */
    .fcrdiv = MCG_SC_FCRDIV(0), /* Fast IRC divide by 1 => 4 MHz */
    .fll_frdiv = MCG_C1_FRDIV(0b011), /* Divide by 256 */
    .fll_factor_fei = KINETIS_MCG_FLL_FACTOR_1464, /* FLL freq = 48 MHz */
    .fll_factor_fee = KINETIS_MCG_FLL_FACTOR_1920, /* FLL freq = 60 MHz */
    .pll_prdiv = MCG_C5_PRDIV0(0b00011), /* Divide by 4 */
    .pll_vdiv  = MCG_C6_VDIV0(0b00110), /* Multiply by 30 => PLL freq = 60 MHz */
};
#define CLOCK_CORECLOCK              (60000000ul)
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
#define LPTMR_CONFIG {           \
    {                            \
        .dev = LPTMR0,           \
        .irqn = LPTMR0_IRQn,     \
    }                            \
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
        .dev    = UART1,
        .freq   = CLOCK_CORECLOCK,
        .pin_rx = GPIO_PIN(PORT_E,  1),
        .pin_tx = GPIO_PIN(PORT_E,  0),
        .pcr_rx = PORT_PCR_MUX(3),
        .pcr_tx = PORT_PCR_MUX(3),
        .irqn   = UART1_RX_TX_IRQn,
        .scgc_addr = &SIM->SCGC4,
        .scgc_bit = SIM_SCGC4_UART1_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_UART,
    },
};

#define UART_0_ISR          (isr_uart1_rx_tx)

#define UART_NUMOF          (sizeof(uart_config) / sizeof(uart_config[0]))
/** @} */

/**
 * @name ADC configuration
 * @{
 */
static const adc_conf_t adc_config[] = {
    /* dev, pin, channel */
    { .dev = ADC0, .pin = GPIO_UNDEF          , .chan =  0 }, /* ADC0_DP0 */
    { .dev = ADC0, .pin = GPIO_UNDEF          , .chan = 19 }, /* ADC0_DM0 */
    { .dev = ADC1, .pin = GPIO_UNDEF          , .chan =  0 }, /* ADC1_DP0 */
    { .dev = ADC1, .pin = GPIO_UNDEF          , .chan = 19 }, /* ADC1_DM0 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  0), .chan =  8 }, /* PTB0 (Arduino A0) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  1), .chan =  9 }, /* PTB1 (Arduino A1) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  1), .chan = 15 }, /* PTC1 (Arduino A2) */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  2), .chan =  4 }, /* PTC2 (Arduino A3) */
};

#define ADC_NUMOF           (sizeof(adc_config) / sizeof(adc_config[0]))
/*
 * K22F ADC reference settings:
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
        .ftm        = FTM0,
        .chan       = {
            { .pin = GPIO_PIN(PORT_A, 1), .af = 3, .ftm_chan = 6 },
            { .pin = GPIO_PIN(PORT_A, 2), .af = 3, .ftm_chan = 7 },
            { .pin = GPIO_PIN(PORT_D, 5), .af = 4, .ftm_chan = 5 },
        },
        .chan_numof = 3,
        .ftm_num    = 0
    }
};

#define PWM_NUMOF           (sizeof(pwm_config) / sizeof(pwm_config[0]))
/** @} */


/**
 * @name   SPI configuration
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
        .pin_miso = GPIO_PIN(PORT_D, 3),
        .pin_mosi = GPIO_PIN(PORT_D, 2),
        .pin_clk  = GPIO_PIN(PORT_D, 1),
        .pin_cs   = {
            GPIO_PIN(PORT_C, 4),
            GPIO_PIN(PORT_D, 4),
            GPIO_UNDEF,
            GPIO_UNDEF,
            GPIO_UNDEF
        },
        .pcr      = GPIO_AF_2,
        .simmask  = SIM_SCGC6_SPI0_MASK
    }
};

#define SPI_NUMOF           (sizeof(spi_config) / sizeof(spi_config[0]))
/** @} */


/**
* @name I2C configuration
* @{
*/
#define I2C_NUMOF                    (1U)
#define I2C_0_EN                     1
/* Low (10 kHz): MUL = 2, SCL divider = 1536, total: 3072 */
#define KINETIS_I2C_F_ICR_LOW        (0x36)
#define KINETIS_I2C_F_MULT_LOW       (1)
/* Normal (100 kHz): MUL = 2, SCL divider = 160, total: 320 */
#define KINETIS_I2C_F_ICR_NORMAL     (0x1D)
#define KINETIS_I2C_F_MULT_NORMAL    (1)
/* Fast (400 kHz): MUL = 1, SCL divider = 80, total: 80 */
#define KINETIS_I2C_F_ICR_FAST       (0x14)
#define KINETIS_I2C_F_MULT_FAST      (0)
/* Fast plus (1000 kHz): MUL = 1, SCL divider = 30, total: 30 */
#define KINETIS_I2C_F_ICR_FAST_PLUS  (0x05)
#define KINETIS_I2C_F_MULT_FAST_PLUS (0)

/* I2C 0 device configuration */
#define I2C_0_DEV                    I2C0
#define I2C_0_CLKEN()                (SIM->SCGC4 |= (SIM_SCGC4_I2C0_MASK))
#define I2C_0_CLKDIS()               (SIM->SCGC4 &= ~(SIM_SCGC4_I2C0_MASK))
#define I2C_0_IRQ                    I2C0_IRQn
#define I2C_0_IRQ_HANDLER            isr_i2c0
/* I2C 0 pin configuration */
#define I2C_0_PORT                   PORTB
#define I2C_0_PORT_CLKEN()           (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK))
#define I2C_0_PIN_AF                 2
#define I2C_0_SDA_PIN                3
#define I2C_0_SCL_PIN                2
#define I2C_0_PORT_CFG               (PORT_PCR_MUX(I2C_0_PIN_AF) | PORT_PCR_ODE_MASK)
/** @} */

/**
* @name RTT and RTC configuration
* @{
*/
#define RTT_NUMOF                    (1U)
#define RTC_NUMOF                    (1U)
#define RTT_DEV                      RTC
#define RTT_IRQ                      RTC_IRQn
#define RTT_IRQ_PRIO                 10
#define RTT_UNLOCK()                 (SIM->SCGC6 |= (SIM_SCGC6_RTC_MASK))
#define RTT_ISR                      isr_rtc
#define RTT_FREQUENCY                (1)
#define RTT_MAX_VALUE                (0xffffffff)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
