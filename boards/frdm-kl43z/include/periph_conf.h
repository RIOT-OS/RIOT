/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_frdm-kl43z
 * @{
 *
 * @file
 * @name        Peripheral MCU configuration for the FRDM-KL43Z
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
     * Core:   8 MHz
     * Bus:    8 MHz
     * Flash:  8 MHz
     */
    .clkdiv1            = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV4(0),
    /* unsure if this RTC load cap configuration is correct */
    .rtc_clc            = RTC_CR_SC8P_MASK | RTC_CR_SC4P_MASK,
    /* Use the 32 kHz system oscillator output as ERCLK32K. */
    .osc32ksel          = SIM_SOPT1_OSC32KSEL(0),
    .clock_flags =
        KINETIS_CLOCK_RTCOSC_EN |
        KINETIS_CLOCK_USE_FAST_IRC |
        KINETIS_CLOCK_MCGIRCLK_EN | /* Used for LPUART clocking */
        KINETIS_CLOCK_MCGIRCLK_STOP_EN |
        0,
    /* Using LIRC8M mode by default */
    .default_mode       = KINETIS_MCG_MODE_LIRC8M,
    /* The crystal connected to EXTAL0 is 32.768 kHz */
    .erc_range          = KINETIS_MCG_ERC_RANGE_LOW,
    .osc_clc            = 0, /* no load cap configuration, rtc_clc overrides this value on KL43Z */
    .fcrdiv             = MCG_SC_FCRDIV(0), /* LIRC_DIV1 divide by 1 => 8 MHz */
    .lirc_div2          = MCG_MC_LIRC_DIV2(0), /* LIRC_DIV2 divide by 1 => 8 MHz */
};
#define CLOCK_CORECLOCK              ( 8000000ul)
#define CLOCK_MCGIRCLK               ( 8000000ul)
#define CLOCK_BUSCLOCK               (CLOCK_CORECLOCK / 1)
/** @} */

/**
 * @name Timer configuration
 * @{
 */
#define PIT_NUMOF               (1U)
#define PIT_CONFIG {             \
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
        }, \
    }
#define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

#define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
#define PIT_ISR_0               isr_pit1
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
        .pin_rx = GPIO_PIN(PORT_A,  1),
        .pin_tx = GPIO_PIN(PORT_A,  2),
        .pcr_rx = PORT_PCR_MUX(2),
        .pcr_tx = PORT_PCR_MUX(2),
        .irqn   = LPUART0_IRQn,
        .scgc_addr = &SIM->SCGC5,
        .scgc_bit = SIM_SCGC5_LPUART0_SHIFT,
        .mode   = UART_MODE_8N1,
        .type   = KINETIS_LPUART,
    },
};
#define UART_NUMOF          ARRAY_SIZE(uart_config)
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
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  0), .chan =  8, .avg = ADC_AVG_MAX }, /* Arduino A0 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  1), .chan =  9, .avg = ADC_AVG_MAX }, /* Arduino A1 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  2), .chan = 15, .avg = ADC_AVG_MAX }, /* Arduino A2 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  3), .chan =  4, .avg = ADC_AVG_MAX }, /* Arduino A3 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  2), .chan = 11, .avg = ADC_AVG_MAX }, /* Arduino A4 */
    { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  1), .chan = 15, .avg = ADC_AVG_MAX }, /* Arduino A5 */
};

#define ADC_NUMOF           ARRAY_SIZE(adc_config)
/*
 * KL43Z ADC reference settings:
 * 0: VREFH/VREFL external pin pair
 * 1: VDDA/VSSA supply pins
 * 2-3: reserved
 */
#define ADC_REF_SETTING     0
/** @} */

/**
* @name I2C configuration
* @{
*/
static const i2c_conf_t i2c_config[] = {
    {
        .i2c = I2C0,
        .scl_pin = GPIO_PIN(PORT_E, 24),
        .sda_pin = GPIO_PIN(PORT_E, 25),
        .freq = CLOCK_CORECLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C0_IRQn,
        .scl_pcr = (PORT_PCR_MUX(5)),
        .sda_pcr = (PORT_PCR_MUX(5)),
    },
    {
        .i2c = I2C1,
        .scl_pin = GPIO_PIN(PORT_E,  1),
        .sda_pin = GPIO_PIN(PORT_E,  0),
        .freq = CLOCK_CORECLOCK,
        .speed = I2C_SPEED_FAST,
        .irqn = I2C1_IRQn,
        .scl_pcr = (PORT_PCR_MUX(6)),
        .sda_pcr = (PORT_PCR_MUX(6)),
    },
};
#define I2C_NUMOF           ARRAY_SIZE(i2c_config)
#define I2C_0_ISR           isr_i2c0
#define I2C_1_ISR           isr_i2c1
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
